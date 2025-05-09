#include "pch.h"

#include <AImGUI.h>
#include <ModLoader.h>
#include <AHooks.h>
#include <HookHelpers.h>

#include <BYardSDK/AGUI2.h>
#include <BYardSDK/THookedRenderD3DInterface.h>
#include <BYardSDK/AGUI2FontManager.h>
#include <BYardSDK/AGameStateController.h>
#include <BYardSDK/ATerrainInterface.h>
#include <BYardSDK/ACamera.h>
#include <BYardSDK/ASteer.h>
#include <BYardSDK/APlayerManager.h>

#include <Toshi/THPTimer.h>
#include <Toshi/TScheduler.h>
#include <File/TFile.h>
#include <ToshiTools/T2CommandLine.h>

TOSHI_NAMESPACE_USING

//-----------------------------------------------------------------------------
// NOTE: Use CALL or CALL_THIS macro to call different methods of the original game by address
// CALL      allows you to call any non-member method
// CALL_THIS allows you to call any member method
//-----------------------------------------------------------------------------

AGUI2TextBox* g_pTextBox       = TNULL;
TBOOL         g_bTopDownCamera = TFALSE;

void AGUI2_MainPostRenderCallback()
{
	if ( g_pTextBox )
	{
		g_pTextBox->PreRender();
		g_pTextBox->Render();
		g_pTextBox->PostRender();
	}
}

MEMBER_HOOK( 0x00463350, ACameraHelper, AGTAStyleCameraHelper_UpdateCamera, TBOOL, TFLOAT a_fDeltaTime )
{
	if ( !g_bTopDownCamera )
		return CallOriginal( a_fDeltaTime );
	
	// Top down camera
	if ( APlayerManager::IsSingletonCreated() )
	{
		if ( AUnitPlayer* pPlayer = APlayerManager::GetSingleton()->GetPlayer( APlayerManager::PLAYER_1 ) )
		{
			if ( ASteer* pSteer = pPlayer->GetSteer() )
			{
				TVector4 vecPosition = *pSteer->GetPosition();
				vecPosition.y -= 18.0f;

				TQuaternion quatRotation;
				quatRotation.SetFromEulerRollPitchYaw( -TMath::PI / 2, 0.0f, 0.0f );

				m_pCamera->m_Matrix.SetFromQuaternion( quatRotation );
				m_pCamera->m_Matrix.SetTranslation( vecPosition );
			}
		}
	}

	return TTRUE;
}

class AModExample : public AModInstance
{
public:
	TBOOL OnLoad() OVERRIDE
	{
		//-----------------------------------------------------------------------------
		// Note: there are two ways to set up function hooks:
		// 1. Using InstallHook to set at any address
		// 2. Using ModCore's AHooks class to set at some specific function
		//-----------------------------------------------------------------------------

		InstallHook<AGTAStyleCameraHelper_UpdateCamera>();

		// Setup hook to the AGUI2::MainPostRenderCallback method to draw our custom text box on top of anything else
		return AHooks::AddHook( Hook_AGUI2_MainPostRenderCallback, HookType_Before, AGUI2_MainPostRenderCallback );
	}

	TBOOL OnUpdate( TFLOAT a_fDeltaTime ) OVERRIDE
	{
		return TTRUE;
	}

	void OnUnload() OVERRIDE
	{
		delete g_pTextBox;
	}

	void OnRenderInterfaceReady( Toshi::TRenderD3DInterface* a_pRenderInterface ) OVERRIDE
	{
		TRenderInterface::SetSingletonExplicit(
		    THookedRenderD3DInterface::GetSingleton()
		);
	}

	void OnAGUI2Ready() OVERRIDE
	{
		TFLOAT fWidth, fHeight;
		AGUI2::GetSingleton()->GetDimensions( fWidth, fHeight );

		// Note: Available fonts are Rekord26 and Rekord18
		AGUI2Font* pFont = AGUI2FontManager::FindFont( "Rekord26" );
		g_pTextBox       = AGUI2TextBox::CreateFromEngine();

		g_pTextBox->Create( pFont, 400.0f );
		g_pTextBox->SetScale( 0.75f );
		g_pTextBox->SetText( L"Example Mod Works!" );
		g_pTextBox->SetColour( TCOLOR( 64, 64, 255 ) );
		g_pTextBox->SetTransform( 0.0f, 0.0f );
		g_pTextBox->SetAlpha( 1.0f );
		g_pTextBox->SetTextAlign( AGUI2Font::TextAlign_Center );
		g_pTextBox->SetInFront();
		g_pTextBox->SetAttachment( AGUI2Element::Anchor_MiddleCenter, AGUI2Element::Pivot_MiddleCenter );
	}

	void OnImGuiRender( AImGUI* a_pImGui ) OVERRIDE
	{
		ImGui::Text( "Hello, world!" );
		ImGui::Checkbox( "Top Down Camera", &g_bTopDownCamera );
	}

	virtual void OnImGuiRenderOverlay( AImGUI* a_pImGui )
	{
		ImGui::SetNextWindowPos( ImVec2( 16.0f, 16.0f ) );
		ImGui::Begin(
			"Example Overlay Window",
			TNULL,
		    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize
		);
		{
			ImGui::Text( "This is an example overlay window." );

			if ( AGameStateController* pGameStateController = AGameStateController::GetSingleton() )
			{
				if ( AGameState* pCurrentState = pGameStateController->GetCurrentState() )
				{
					ImGui::Text( "Current GameState: %s", pCurrentState->GetClass()->GetName() );
				}
			}

			if ( ATerrainInterface* pTerrainInterface = ATerrainInterface::GetSingleton() )
			{
				ImGui::Text(
				    "Current Terrain: %s (section: %s)",
				    ABYardTerrainManager::ms_aTerrains[ ABYardTerrainManager::ms_eCurrentLevel ].szName,
				    pTerrainInterface->m_pTerrainVIS->m_pSections[ pTerrainInterface->m_iCurrentGroup ].m_szName
				);
			}

			ImGui::End();
		}

	}

	TBOOL HasSettingsUI() OVERRIDE
	{
		return TTRUE;
	}

	virtual TBOOL IsOverlayVisible() OVERRIDE
	{
		return TTRUE;
	}
};

extern "C"
{
	MODLOADER_EXPORT AModInstance* CreateModInstance( const T2CommandLine* a_pCommandLine )
	{
		// TODO: Specify max memory size to allocate for the mod
		TMemory::Initialise( 1 * 1024 * 1024, 0 );

		TUtil::TOSHIParams toshiParams;
		toshiParams.szCommandLine = "";
		toshiParams.szLogFileName = "mod-example";
		toshiParams.szLogAppName  = "ModExample";

		TUtil::ToshiCreate( toshiParams );

		return new AModExample();
	}

	MODLOADER_EXPORT const TCHAR* GetModAutoUpdateURL()
	{
		// TODO: Replace with a JSON file containing info about versions
		// Example: https://github.com/InfiniteC0re/OpenBarnyard/releases/download/speedrunhelper/update.json

		return TNULL;
	}

	MODLOADER_EXPORT const TCHAR* GetModName()
	{
		return "Mod Example";
	}

	MODLOADER_EXPORT TUINT32 GetModVersion()
	{
		return TVERSION( 1, 0 );
	}
}
