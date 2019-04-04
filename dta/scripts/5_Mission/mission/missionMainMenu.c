class MissionMainMenu extends MissionBase
{
	private UIScriptedMenu m_mainmenu;
	private ref DayZIntroScenePC m_IntroScenePC;
	private ref DayZIntroSceneXbox m_IntroSceneXbox;
	bool m_NoCutscene;

	override void OnInit()
	{
		if (!m_NoCutscene)
		{
			CreateIntroScene();
		}
		
		if (!m_mainmenu)
		{
			#ifdef PLATFORM_CONSOLE
				m_mainmenu = UIScriptedMenu.Cast( g_Game.GetUIManager().EnterScriptedMenu( MENU_TITLE_SCREEN, null ) );
			#else
				m_mainmenu = UIScriptedMenu.Cast( g_Game.GetUIManager().EnterScriptedMenu( MENU_MAIN, null ) );
			#endif
		}
	}
	
	override void Reset()
	{
		#ifdef PLATFORM_CONSOLE
			delete m_IntroSceneXbox;
		#else
			delete m_IntroScenePC;
		#endif
		
		CreateIntroScene();
	}
	
	DayZIntroScenePC GetIntroScenePC()
	{
		#ifdef PLATFORM_CONSOLE
			Error("missionMainMenu->GetIntroScenePC on PLATFORM_CONSOLE is not implemented!");
			return null;
		#else
			return m_IntroScenePC;
		#endif
	}
	
	DayZIntroSceneXbox GetIntroSceneXbox()
	{
		#ifdef PLATFORM_CONSOLE
			return m_IntroSceneXbox;
		#else
			Error("missionMainMenu->GetIntroScenePC on PLATFORM_PC is not implemented!");
			return null;
		#endif
	}

	void CreateIntroScene()
	{		
#ifdef PLATFORM_CONSOLE
		m_IntroSceneXbox = new DayZIntroSceneXbox;
#else
		Print("misssionMainMenu CreateIntroScene");
		m_IntroScenePC = new DayZIntroScenePC;
#endif
	}

	override void OnMissionStart()
	{
		if (m_mainmenu)
		{
			//m_mainmenu.FadeIn(2.0);	//Fade in method is currently commented in MainMenu class
		}
		g_Game.GetUIManager().ShowUICursor(true);
		g_Game.SetMissionState( DayZGame.MISSION_STATE_MAINMENU );
	}
	
	override void OnMissionFinish()
	{
		if( m_mainmenu )
			m_mainmenu.Cleanup();
		GetGame().GetUIManager().CloseAll();
		m_mainmenu = NULL;
		
		m_IntroScenePC = null;
		m_IntroSceneXbox = null;
		g_Game.GetUIManager().ShowUICursor(false);
	}

	override void OnUpdate(float timeslice)
	{
		//Print("("+ GetGame().GetTime() +") ____OnUpdate: " + timeslice);
		
		if ( g_Game.IsLoading() )
		{
			return;
		}
				
		if (m_IntroScenePC)
		{
			m_IntroScenePC.Update();
		}
	}
	
	
	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		
		if ( key == KeyCode.KC_Q )
		{
			/*
			array<int> list = new  array<int>();
			
			Print("Start--------");
			list.Debug();
			
			int i = 0;
			while ( i < 20 )
			{
				SortedInsert(list, Math.RandomIntInclusive(0, 20));
				i += 1;
			}
			
			
			
			Print("End--------");
			list.Debug();
			*/
		}
	}
	
	int SortedInsert( array<int> list, int number )
	{
		int find_number = number;
		int index_min = 0;
		int index_max = list.Count() - 1;
		int target_index = Math.Floor( index_max / 2 );
		
		if ( index_max == -1 )
		{
			list.Insert( number );
			return 0;
		}
		
		while ( true )
		{
			int target_value = list[target_index];
			
			if ( find_number == target_value || ((index_max - index_min) <= 1) )
			{
				for ( int i = index_min; i <= index_max; i++ )
				{
					if ( find_number <= list[i] )
					{
						list.InsertAt( find_number, i );
						return i;
					}
				}
				
				index_max++;
				list.InsertAt( find_number, index_max );
				return target_index;
			}
			else if ( find_number < target_value )
			{
				index_max = target_index;
				target_index = Math.Floor( target_index / 2 );
			}
			else if ( find_number > target_value )
			{
				index_min = target_index;
				target_index += Math.Floor( (index_max - index_min) / 2 );
			}			
		}
		
		return target_index;
	}
}
