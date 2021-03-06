class HandsContainer: Container
{
	protected bool											m_Hidden;
	protected EntityAI										m_Entity;
	protected ref HandsHeader								m_CollapsibleHeader;
	protected ref HandsPreview								m_HandsPreview;
	
	protected ref Attachments								m_Atts;
	protected ref CargoContainer							m_CargoGrid;
	
	protected ref map<EntityAI, ref CargoContainer>			m_AttachmentCargos;
	protected ref map<EntityAI, ref AttachmentsWrapper>		m_AttachmentAttachmentsContainers;
	protected ref map<EntityAI, ref Attachments>			m_AttachmentAttachments;
	
	EntityAI m_am_entity1, m_am_entity2;
	
	protected ScrollWidget									m_ScrollWidget;

	void HandsContainer( LayoutHolder parent )
	{
		m_CollapsibleHeader = new HandsHeader( this, "CollapseButtonOnMouseButtonDown" );
		GetMainWidget().SetFlags( WidgetFlags.IGNOREPOINTER );
		m_MainWidget = m_MainWidget.FindWidget( "body" );
		GetMainWidget().SetFlags( WidgetFlags.IGNOREPOINTER );
		m_Body = new array<ref LayoutHolder>;
		m_HandsPreview		= new HandsPreview( this );
		ItemManager.GetInstance().SetHandsPreview( m_HandsPreview );
		m_Body.Insert( m_HandsPreview );
		
		m_AttachmentCargos					= new map<EntityAI, ref CargoContainer>;
		m_AttachmentAttachmentsContainers	= new map<EntityAI, ref AttachmentsWrapper>;
		m_AttachmentAttachments				= new map<EntityAI, ref Attachments>;
		
		m_ScrollWidget						= ScrollWidget.Cast( m_RootWidget.GetParent().GetParent() );
	}
	
	HandsHeader GetHeader()
	{
		return m_CollapsibleHeader;
	}
	
	override bool IsLastIndex()
	{
		return m_ActiveIndex == m_Body.Count();
	}
	
	override bool IsFirstContainerFocused()
	{
		return IsFirstIndex();
	}
	
	override bool IsLastContainerFocused()
	{
		return IsLastIndex();
	}
	
	override void SetNextActive()
	{
		ItemManager.GetInstance().HideTooltip( );
		if( ItemManager.GetInstance().IsMicromanagmentMode() )
		{
			ItemManager.GetInstance().SetItemMoving( true );
		}
		
		if( !IsLastIndex() )
		{
			if( GetFocusedContainer() )
				GetFocusedContainer().SetActive( false );
			++m_ActiveIndex;
			if( GetFocusedContainer() )
				GetFocusedContainer().SetActive( true );
		}
		
		SetFocusToIndex();
	}

	override void SetPreviousActive( bool force = false )
	{
		if( ItemManager.GetInstance().IsMicromanagmentMode() )
		{
			ItemManager.GetInstance().SetItemMoving( true );
		}
		
		if( !IsFirstIndex() )
		{
			if( GetFocusedContainer() )
				GetFocusedContainer().SetActive( false );
			m_ActiveIndex--;
			if( GetFocusedContainer() )
				GetFocusedContainer().SetActive( true );
		}
		
		SetFocusToIndex();
	}
	
	override Container GetFocusedContainer()
	{
		int index = m_ActiveIndex - 2;
		int attachment_start_index = -1;
		int cargo_start_index = -1;
		int attachment_end_index = -1;
		int cargo_end_index = -1;
		
		if( m_Atts || m_AttachmentAttachments.Count() > 0 )
		{
			attachment_start_index = 0;
			if( m_Atts )
				attachment_end_index++;
			attachment_end_index += m_AttachmentAttachments.Count();
		}
		
		if( m_CargoGrid || m_AttachmentCargos.Count() > 0 )
		{
			cargo_start_index = attachment_end_index + 1;
			if( m_CargoGrid )
				cargo_end_index++;
			cargo_end_index += cargo_start_index + m_AttachmentCargos.Count();
		}
		
		if( index == -1 )
		{
			
		}
		else if( index.InRange( 0, attachment_end_index ) )
		{
			if( m_Atts )
			{
				if( index == 0 )
				{
					return m_Atts.GetWrapper();
				}
				else
				{
					return m_AttachmentAttachments.GetElement( index - 1 ).GetWrapper();
				}
			}
			else
			{
				return m_AttachmentAttachments.GetElement( index ).GetWrapper();
			}
		}
		else if( index.InRange( cargo_start_index, cargo_end_index ) )
		{
			if( m_CargoGrid )
			{
				if( index == cargo_start_index )
				{
					return m_CargoGrid;
				}
				else
				{
					return m_AttachmentCargos.GetElement( index - 1 - cargo_start_index );
				}
			}
			else
			{
				return m_AttachmentCargos.GetElement( index - cargo_start_index );
			}
		}
		return null;
	}
	
	void SetFocusToIndex()
	{
		int index = m_ActiveIndex - 2;
		int attachment_start_index = -1;
		int cargo_start_index = -1;
		int attachment_end_index = -1;
		int cargo_end_index = -1;
		
		if( m_Atts || m_AttachmentAttachments.Count() > 0 )
		{
			attachment_start_index = 0;
			if( m_Atts )
				attachment_end_index++;
			attachment_end_index += m_AttachmentAttachments.Count();
		}
		
		if( m_CargoGrid || m_AttachmentCargos.Count() > 0 )
		{
			cargo_start_index = attachment_end_index + 1;
			if( m_CargoGrid )
				cargo_end_index++;
			cargo_end_index += cargo_start_index + m_AttachmentCargos.Count();
		}
		
		if( m_ActiveIndex > m_Body.Count() )
		{
			m_ActiveIndex = m_Body.Count();
		}
		
		if( index == -1 )
		{
			#ifdef PLATFORM_CONSOLE
			if( m_MainWidget.FindAnyWidget("Selected") )
				m_MainWidget.FindAnyWidget("Selected").Show( true );
			#endif
			m_ScrollWidget.VScrollToPos01( 0 );
		}
		else
		{
			if( index.InRange( 0, attachment_end_index ) )
			{
				if( m_Atts )
				{
					if( index == 0 )
					{
						m_Atts.SetDefaultFocus();
						SetFocusedContainer( m_Atts.GetWrapper() );
					}
					else
					{
						m_AttachmentAttachments.GetElement( index - 1 ).SetDefaultFocus();
						SetFocusedContainer( m_AttachmentAttachments.GetElement( index - 1 ).GetWrapper() );
					}
				}
				else
				{
					m_AttachmentAttachments.GetElement( index ).SetDefaultFocus();
					SetFocusedContainer( m_AttachmentAttachments.GetElement( index ).GetWrapper() );
				}
			}
			else if( index.InRange( cargo_start_index, cargo_end_index ) )
			{
				if( m_CargoGrid )
				{
					if( index == cargo_start_index )
					{
						m_CargoGrid.SetDefaultFocus();
						SetFocusedContainer( m_CargoGrid );
					}
					else
					{
						m_AttachmentCargos.GetElement( index - 1 - cargo_start_index ).SetDefaultFocus();
						SetFocusedContainer( m_AttachmentCargos.GetElement( index - 1 - cargo_start_index ) );
					}
				}
				else
				{
					m_AttachmentCargos.GetElement( index - cargo_start_index ).SetDefaultFocus();
					SetFocusedContainer( m_AttachmentCargos.GetElement( index - cargo_start_index ) );
				}
			}
			
			if( m_MainWidget.FindAnyWidget("Selected") )
				m_MainWidget.FindAnyWidget("Selected").Show( false );
			ScrollToActiveContainer( GetFocusedContainer() );
		}
	}
	
	void ScrollToActiveContainer( Container active_container )
	{
		if( !active_container )
			return;
		float x, y, y_s;
		m_ScrollWidget.GetScreenPos( x, y );
		m_ScrollWidget.GetScreenSize( x, y_s );
		float amount;
		float next_pos	= active_container.GetFocusedContainerYScreenPos() + active_container.GetFocusedContainerHeight();
			
		if( next_pos > ( y + y_s ) )
		{
			amount	= y + active_container.GetFocusedContainerYScreenPos();
			m_ScrollWidget.VScrollToPos( m_ScrollWidget.GetVScrollPos() + active_container.GetFocusedContainerHeight() + 2 );
		}
		else if( active_container.GetFocusedContainerYScreenPos() < y )
		{
			amount = active_container.GetFocusedContainerYScreenPos() - y;
			m_ScrollWidget.VScrollToPos( m_ScrollWidget.GetVScrollPos() + amount - 2 );
		}
	}
	
	override void Insert( LayoutHolder container, int pos = -1 )
	{
		super.Insert( container, pos );
	}

	void MouseClick2( Widget w, int x, int y, int button )
	{
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );
		ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		
		if( !item_preview )
		{
			return;
		}
		
		EntityAI item = item_preview.GetItem();
		InventoryItem itemAtPos = InventoryItem.Cast( item );
		
		if( item )
		{
			if ( button == MouseState.MIDDLE )
			{
				InspectItem( itemAtPos );
			}
		}
	}
		
	void DraggingOverHeader2(Widget w, int x, int y, Widget receiver )
	{
		DraggingOverHeader(w, x, y, receiver );
	}
	
	override bool TransferItem()
	{
		if( m_ActiveIndex == 1 )
		{
			EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
			if( item_in_hands && GetGame().GetPlayer().GetHumanInventory().CanRemoveEntityInHands() )
			{
				if( GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.CARGO, item_in_hands ) )
				{
					m_MainWidget.FindAnyWidget("Selected").Show( false );
					m_MainWidget.FindAnyWidget("hands_preview_root").SetAlpha( 0.7 );
					return true;
				}
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().TransferItem();
		}
		return false;
	}
	
	override bool CanEquip()
	{
		if( m_ActiveIndex == 1 )
		{
			EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
			if( item_in_hands.IsInherited( Magazine ) )
			{
				return false;
			}
			InventoryLocation il = new InventoryLocation;
			bool found = GetGame().GetPlayer().GetInventory().FindFreeLocationFor(item_in_hands,FindInventoryLocationType.ATTACHMENT,il);
			if (found)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().CanEquip();
		}
		return false;
	}
	
	override bool EquipItem()
	{
		if( m_ActiveIndex == 1 )
		{
			ItemBase item_in_hands = ItemBase.Cast( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands() );
			if( item_in_hands )
			{
				if( item_in_hands.CanBeSplit() )
				{
					item_in_hands.OnRightClick();
				}
				else if( GetGame().GetPlayer().GetHumanInventory().CanRemoveEntityInHands() )
				{
					if( GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.ATTACHMENT, item_in_hands ) )
					{
						m_MainWidget.FindAnyWidget("Selected").Show( false );
						m_MainWidget.FindAnyWidget("hands_preview_root").SetAlpha( 0.7 );
						return true;
					}
				}
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().EquipItem();
		}
		return false;
	}
	
	override void SetActive( bool active )
	{
		m_IsActive = active;
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
		GetHeader().SetActive( active );
		m_ActiveIndex = 1;
		if( item_in_hands )
		{
			if( m_MainWidget.FindAnyWidget( "Selected" ) )
			{
				m_MainWidget.FindAnyWidget( "Selected" ).Show( active );
			}
			
			if( active )
			{
				float x, y;
				GetMainWidget().GetScreenPos( x, y );
				ItemManager.GetInstance().PrepareTooltip( item_in_hands, x, y );
			}
		}
		else
		{
			if( active )
				m_MainWidget.FindAnyWidget( "hands_preview_root" ).SetAlpha( 0.85 );
			else
				m_MainWidget.FindAnyWidget( "hands_preview_root" ).SetAlpha( 0.7 );
		}
	}
	
	override void Refresh()
	{
		super.Refresh();
		SetActive( m_IsActive );
	}
	
	override bool IsItemActive()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		ItemBase item = ItemBase.Cast( player.GetHumanInventory().GetEntityInHands() );
		if( item == null )
		{
			return false;
		}
		return !IsEmpty() && ( !QuantityConversions.HasItemQuantity( item )  || ( QuantityConversions.HasItemQuantity( item ) && !item.CanBeSplit() ) );
	}
	
	override bool IsItemWithQuantityActive()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		ItemBase item = ItemBase.Cast( player.GetHumanInventory().GetEntityInHands() );
		if( item == null )
		{
			return false;
		}
		return !IsEmpty() && QuantityConversions.HasItemQuantity( item ) && item.CanBeSplit();
	}
	
	override bool IsEmpty()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		return player.GetHumanInventory().GetEntityInHands() == null;
	}
	
	override bool TransferItemToVicinity()
	{
		if( m_ActiveIndex == 1 )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
			if( item_in_hands && player.CanDropEntity( item_in_hands ) && GetGame().GetPlayer().GetHumanInventory().CanRemoveEntityInHands() )
			{
				if( player.PredictiveDropEntity( item_in_hands ) )
				{
					m_MainWidget.FindAnyWidget("Selected").Show( false );
					m_MainWidget.FindAnyWidget("hands_preview_root").SetAlpha( 0.7 );
					return true;
				}
			}
		}
		else if( GetFocusedContainer() )
		{
				return GetFocusedContainer().TransferItemToVicinity();
		}
		return false;
	}
	
	override bool SelectItem()
	{
		Man player = GetGame().GetPlayer();
		EntityAI item_in_hands;
		if( m_ActiveIndex == 1 )
			item_in_hands = player.GetHumanInventory().GetEntityInHands();
		else
			item_in_hands = GetFocusedContainer().GetFocusedEntity();
		
		if( item_in_hands )
		{
			ItemManager.GetInstance().SetSelectedItem( item_in_hands, null, null );
			return true;
		}
		return false;
	}
	
	EntityAI item_to_be_swap;
	EntityAI prev_item;
	override bool Select()
	{
		if( m_ActiveIndex == 1 )
		{
			Man player = GetGame().GetPlayer();
			EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
			if( ItemManager.GetInstance().IsMicromanagmentMode() )
			{
				EntityAI selected_item = ItemManager.GetInstance().GetSelectedItem();
				if( selected_item && selected_item.GetInventory().CanRemoveEntity() )
				{
					if( item_in_hands && item_in_hands.GetInventory().CanRemoveEntity() )
					{
						if( GameInventory.CanSwapEntities( item_in_hands, selected_item ) )
						{
							player.PredictiveSwapEntities( item_in_hands, selected_item );
							item_to_be_swap = selected_item;
							return true;
						}
					}
					else
					{
						if( player.GetHumanInventory().CanAddEntityInHands( selected_item ) )
						{
			 				ItemBase item_base = ItemBase.Cast( selected_item );
							float stackable = item_base.ConfigGetFloat("varStackMax");
							if( stackable == 0 || item_base.GetQuantity() <= stackable )
							{
								GetGame().GetPlayer().PredictiveTakeEntityToHands( item_base );		
								return true;
							}
							else if( stackable != 0 && stackable <= item_base.GetQuantity() )
							{
								item_base.SplitIntoStackMaxHandsClient( PlayerBase.Cast( GetGame().GetPlayer() ) );
								return true;
							}
						}
					}
				}
			}
		}
		else if( GetFocusedContainer() )
		{
			GetFocusedContainer().Select();
			return false;
		}
		
		return false;
	}
	
	EntityAI GetItemPreviewItem( Widget w )
	{
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if( !ipw )
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		
		if( !ipw )
		{
			ipw = ItemPreviewWidget.Cast( w );
		}
		
		if( !ipw )
		{
			return null;
		}
		
		return ipw.GetItem();
	}

	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if( w == null )
		{
			return;
		}
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );

		ItemPreviewWidget w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		if( w_ipw == null )
		{
			w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		}
		if( w_ipw == null )
		{
			return;
		}

		PlayerBase p = PlayerBase.Cast( GetGame().GetPlayer() );
		InventoryItem receiver_entity = InventoryItem.Cast( p.GetHumanInventory().GetEntityInHands() );
		InventoryItem w_entity = InventoryItem.Cast( w_ipw.GetItem() );
		if( !w_entity )
		{
			return;
		}

		if( receiver_entity != null && GameInventory.CanSwapEntities( receiver_entity, w_entity ) )
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
		}
		else
		{
			//if( m_HandsIcon )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
			}
			/*else
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
				ItemManager.GetInstance().ShowSourceDropzone( w_entity );
			}*/
		}
	}

	void OnDropReceivedFromHeader2( Widget w, int x, int y, Widget receiver )
	{
		TakeAsAttachment( w, receiver );
	}
	
	void OnPerformCombination(int combinationFlags)
	{
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( m_am_entity1 == null || m_am_entity2 == null ) return;

		if ( combinationFlags == InventoryCombinationFlags.NONE ) return;

		if ( combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			Magazine mag;
			if ( Class.CastTo(mag,  m_am_entity2 ) )
			{
				m_player.GetWeaponManager().LoadBullet( mag );
				return;
			}
		}

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			if( m_Atts )
			{
				m_Atts.Remove();
				delete m_Atts;
			}
			if( m_am_entity2.GetSlotsCountCorrect() > 0 )
			{
				m_Atts = new Attachments( this, m_am_entity2 );
				m_Atts.InitAttachmentGrid( 1 );

			}

			Refresh();
			m_Parent.Refresh();

			Weapon_Base wpn1;
			Magazine mag1;
			if ( Class.CastTo(wpn1,  m_am_entity1 ) && Class.CastTo(mag1,  m_am_entity2 ) )
			{
				if( m_player.GetWeaponManager().CanAttachMagazine(wpn1, mag1) )
				{
					m_player.GetWeaponManager().AttachMagazine(mag1);
					return;
				}
			}
			m_player.PredictiveTakeEntityToTargetAttachment( m_am_entity1, m_am_entity2 );
		}

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO )
		{
			m_player.PredictiveTakeEntityToTargetCargo( m_am_entity1, m_am_entity2 );
		}

		if ( combinationFlags & InventoryCombinationFlags.SWAP )
		{
			Magazine swapmag1;
			Magazine swapmag2;
			if (Class.CastTo(swapmag1,  m_am_entity1 ) && Class.CastTo(swapmag2,  m_am_entity2 ) )
			{
				if( m_player.GetWeaponManager().CanSwapMagazine( Weapon_Base.Cast( swapmag1.GetParent() ), Magazine.Cast( swapmag2 ) ) )
				{
					m_player.GetWeaponManager().SwapMagazine(swapmag2);
					return;
				}

				if( m_player.GetWeaponManager().CanSwapMagazine(Weapon_Base.Cast( swapmag2.GetParent() ), Magazine.Cast( swapmag1 )) )
				{
					m_player.GetWeaponManager().SwapMagazine(swapmag1);
					return;
				}
			}
			if( !m_player.PredictiveSwapEntities( m_am_entity1, m_am_entity2 ) && m_player.GetHumanInventory().CanAddEntityInHands( m_am_entity1 ) )
			{
				m_player.PredictiveTakeEntityToHands( m_am_entity1 );
			}
		}

		if ( combinationFlags & InventoryCombinationFlags.TAKE_TO_HANDS )
		{
			if( m_Atts )
			{
				m_Atts.Remove();
				delete m_Atts;
			}

			if( m_am_entity2.GetSlotsCountCorrect() > 0 )
			{
				m_Atts = new Attachments( this, m_am_entity2 );
				m_Atts.InitAttachmentGrid( 1 );

			}
			Refresh();
			m_Parent.Refresh();
			
			if( m_player.GetHumanInventory().CanAddEntityInHands( m_am_entity2 ) )
			{
				m_player.PredictiveTakeEntityToHands( m_am_entity2 );
			}
		}
	}

	void ShowActionMenuCombine( EntityAI entity1, EntityAI entity2, int combinationFlags )
	{
		if ( entity1 && entity2 ) PrintString( "Showing action menu for " + entity1.GetDisplayName() + " and " + entity2.GetDisplayName() );

		ContextMenu cmenu = GetGame().GetUIManager().GetMenu().GetContextMenu();
		m_am_entity1 = entity1;
		m_am_entity2 = entity2;
		cmenu.Hide();
		cmenu.Clear();

		if( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2 )
		{
			ItemBase entity = ItemBase.Cast( entity1 );
			entity.CombineItemsClient( ItemBase.Cast( entity2 ) );
			return;
		}

		if ( entity1 == null || entity2 == null ) return;

		if ( combinationFlags == InventoryCombinationFlags.NONE ) return;

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			cmenu.Add( "#inv_context_add_as_attachment", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.ADD_AS_ATTACHMENT ) );
		}
		if ( combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			cmenu.Add( "#inv_context_load_chamber", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.LOAD_CHAMBER ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO )
		{
			cmenu.Add( "#inv_context_add_as_cargo", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.ADD_AS_CARGO ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.SWAP )
		{
			cmenu.Add( "#inv_context_swap", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.SWAP ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.TAKE_TO_HANDS )
		{
			cmenu.Add( "#inv_context_take_to_hands", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.TAKE_TO_HANDS ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY )
		{
			cmenu.Add( "#inv_context_combine", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.COMBINE_QUANTITY ) );
		}

		int m_am_pos_x,  m_am_pos_y;
		GetMousePos( m_am_pos_x, m_am_pos_y );
		m_am_pos_x -= 5;
		m_am_pos_y -= 5;

		cmenu.Show( m_am_pos_x, m_am_pos_y );
	}

	bool OnIconDrag( Widget w, int x, int y )
	{
		ItemManager.GetInstance().SetIsDragging( true );
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );

		float icon_x, icon_y, x_content, y_content;
		int m_sizeX, m_sizeY;

		InventoryItem i_item = InventoryItem.Cast( ipw.GetItem() );
		GetGame().GetInventoryItemSize( i_item, m_sizeX, m_sizeY );

		GetMainWidget().GetScreenSize( x_content, y_content );
		icon_x = x_content / 10;
		icon_y = x_content / 10;
		w.SetFlags( WidgetFlags.EXACTSIZE );
		w.SetSize( icon_x, icon_y );
		w.SetSize( icon_x * m_sizeX - 1 , icon_y * m_sizeY + m_sizeY - 1 );

		if( !ipw.GetItem() )
		{
			return false;
		}

		name.Replace( "Render", "Col" );
		w.FindAnyWidget( name ).Show( true );
		name.Replace( "Col", "Selected" );
		w.FindAnyWidget( name ).Show( true );
		ItemManager.GetInstance().SetDraggedItem( ipw.GetItem() );

		return false;
	}

	bool OnIconDrop( Widget w, int x, int y, Widget reciever )
	{
		ItemManager.GetInstance().SetIsDragging( false );
		w.ClearFlags( WidgetFlags.EXACTSIZE );
		w.SetSize( 1, 1 );
		string name = w.GetName();
		name.Replace( "PanelWidget", "Col" );
		w.FindAnyWidget( name ).Show( false );
		name.Replace( "Col", "Selected" );
		w.FindAnyWidget( name ).Show( false );
		w.FindAnyWidget( name ).SetColor( ARGBF( 1, 1, 1, 1 ) );
		ItemManager.GetInstance().SetDraggedItem( null );

		return false;
	}

	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		TakeAsAttachment( w, receiver );
	}

	override void RefreshQuantity( EntityAI item_to_refresh )
	{
		HandsPreview preview = HandsPreview.Cast( m_Body.Get( 0 ) );
		preview.RefreshQuantity( item_to_refresh );
	}

	int GetCombinationFlags( EntityAI entity1, EntityAI entity2 )
	{
		int flags = 0;
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI entity_in_hands = m_player.GetHumanInventory().GetEntityInHands();

		if ( !entity1 || !entity2 ) return flags;

		Magazine swapmag1;
		Magazine swapmag2;
		bool skipSwap = false;

		if( Class.CastTo(swapmag1, entity1) && Class.CastTo(swapmag2, entity2) )
		{
			Weapon_Base parentWpn;

			if(	Class.CastTo(parentWpn, swapmag1.GetHierarchyParent())	)
			{
				skipSwap = true;
				if( m_player.GetWeaponManager().CanSwapMagazine(parentWpn,swapmag2) )
					flags = flags | InventoryCombinationFlags.SWAP;
			}

			if(	Class.CastTo(parentWpn, swapmag2.GetHierarchyParent())	)
			{
				skipSwap = true;
				if( m_player.GetWeaponManager().CanSwapMagazine(parentWpn,swapmag1) )
					flags = flags | InventoryCombinationFlags.SWAP;

			}
		}

		if ( !skipSwap )
		{
			if ( entity1 == m_player.GetHumanInventory().GetEntityInHands() ) flags = flags | InventoryCombinationFlags.TAKE_TO_HANDS;
			else if ( GameInventory.CanSwapEntities( entity1, entity2 ) )
			{
				if ( !entity1.IsInherited( ZombieBase ) && !entity1.IsInherited( Car ) )
				{
					flags = flags | InventoryCombinationFlags.SWAP;
				}
			}
		}

		if ( entity1.IsInherited( ItemBase) && entity2.IsInherited( ItemBase ) )
		{
			ItemBase ent1 = ItemBase.Cast( entity1 );
			if ( ent1.CanBeCombined( ItemBase.Cast( entity2 ), PlayerBase.Cast( m_player ) ) ) flags = flags | InventoryCombinationFlags.COMBINE_QUANTITY2;
		}

		Weapon_Base wpn;
		Magazine mag;
		if ( Class.CastTo(wpn,  entity1 ) && Class.CastTo(mag,  entity2 ) )
		{
			int muzzleIndex = wpn.GetCurrentMuzzle();
			if ( m_player.GetWeaponManager().CanLoadBullet(wpn, mag) )
			{
				flags = flags | InventoryCombinationFlags.LOAD_CHAMBER;
			}
			else if ( m_player.GetWeaponManager().CanAttachMagazine(wpn, mag) )
			{
				flags = flags | InventoryCombinationFlags.ATTACH_MAGAZINE;
			}
		}
		else if ( entity1.GetInventory().CanAddAttachment( entity2 ) )
		{
			if ( !entity1.IsInherited( ZombieBase ) && !entity1.IsInherited( Car ) && !entity2.IsInherited( ZombieBase ) && !entity2.IsInherited( Car ) )
			{
				flags = flags | InventoryCombinationFlags.ADD_AS_ATTACHMENT;
			}
		}
		if ( entity1.GetInventory().CanAddEntityInCargo( entity2 ) ) flags = flags | InventoryCombinationFlags.ADD_AS_CARGO;

		if( entity1 == m_player.GetHumanInventory().GetEntityInHands() || entity2 == m_player.GetHumanInventory().GetEntityInHands() )
		{
			if( GetRecipeCount( false, entity1, entity2 ) > 0 )
			{
				flags = flags | InventoryCombinationFlags.RECIPE_HANDS;
			}

			ActionManagerClient amc;
			Class.CastTo(amc, m_player.GetActionManager());
			if( entity1 == m_player.GetHumanInventory().GetEntityInHands() )
			{
				if( amc.CanSetActionFromInventory( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ) ) )
				{
					flags = flags | InventoryCombinationFlags.SET_ACTION;
				}
			}
			else
			{
				if( amc.CanSetActionFromInventory( ItemBase.Cast( entity2 ), ItemBase.Cast( entity1 ) ) )
				{
					flags = flags | InventoryCombinationFlags.SET_ACTION;
				}
			}
		}

		if( GetRecipeCount( true, entity1, entity2 ) > 0 )
		{
			flags = flags | InventoryCombinationFlags.RECIPE_ANYWHERE;
		}
		return flags;
	}

	int GetRecipeCount( bool recipe_anywhere, EntityAI entity1, EntityAI entity2 )
	{
		PluginRecipesManager plugin_recipes_manager = PluginRecipesManager.Cast( GetPlugin( PluginRecipesManager ) );
		return plugin_recipes_manager.GetValidRecipes( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ), null, PlayerBase.Cast( GetGame().GetPlayer() ) );
	}
	
	void RemoveItem()
	{
		m_Entity = null;
		m_CollapsibleHeader.ShowCollapseButtons( false, true );
	}
	
	void DestroyAtt()
	{
		if( m_Atts )
		{
			m_Atts.Remove();
			m_Body.RemoveItem( m_Atts.GetWrapper() );
			delete m_Atts;
		}
		
		if( m_AttachmentAttachmentsContainers )
		{
			foreach( EntityAI ent, AttachmentsWrapper att : m_AttachmentAttachmentsContainers )
			{
				m_Body.RemoveItem( att );
				if( m_AttachmentAttachments.Get( ent ) )
					m_AttachmentAttachments.Get( ent ).Remove();
				m_AttachmentAttachments.Remove( ent );
				delete att;
			}
		}
		
		m_AttachmentAttachmentsContainers.Clear();
		Refresh();
		m_Parent.Refresh();
	}
	
	override void UnfocusGrid()
	{
		if( m_Atts )
		{
			m_Atts.UnfocusAll();
		}
			
		if( m_CargoGrid )
		{
			m_CargoGrid.UnfocusAll();
		}
		
		if( m_AttachmentCargos )
		{
			foreach( EntityAI e1, CargoContainer cargo : m_AttachmentCargos )
			{
				cargo.UnfocusAll();
					
			}
		}
		
		if( m_AttachmentCargos )
		{
			foreach( EntityAI e2, Attachments att : m_AttachmentAttachments )
			{
				att.UnfocusAll();
			}
		}
	}
	
	override void UnfocusAll()
	{
		UnfocusGrid();
	}
	
	override void MoveGridCursor( int direction )
	{
		int index = m_ActiveIndex - 2;
		int attachment_start_index = -1;
		int cargo_start_index = -1;
		int attachment_end_index = -1;
		int cargo_end_index = -1;
		
		if( m_Atts || m_AttachmentAttachments.Count() > 0 )
		{
			attachment_start_index = 0;
			if( m_Atts )
				attachment_end_index++;
			attachment_end_index += m_AttachmentAttachments.Count();
		}
		
		if( m_CargoGrid || m_AttachmentCargos.Count() > 0 )
		{
			cargo_start_index = attachment_end_index + 1;
			if( m_CargoGrid )
				cargo_end_index++;
			cargo_end_index += cargo_start_index + m_AttachmentCargos.Count();
		}
		
		if( index == -1 )
		{
			if( direction == Direction.UP )
			{
				SetPreviousActive();
			}
			else if( direction == Direction.DOWN )
			{
				SetNextActive();
			}
		}
		else if( index.InRange( 0, attachment_end_index ) )
		{
			if( m_Atts )
			{
				if( index == 0 )
				{
					m_Atts.MoveGridCursor( direction );
				}
				else
				{
					m_AttachmentAttachments.GetElement( index - 1 ).MoveGridCursor( direction );
				}
			}
			else
			{
				m_AttachmentAttachments.GetElement( index ).MoveGridCursor( direction );
			}
		}
		else if( index.InRange( cargo_start_index, cargo_end_index ) )
		{
			if( m_CargoGrid )
			{
				if( index == cargo_start_index )
				{
					m_CargoGrid.MoveGridCursor( direction );
				}
				else
				{
					m_AttachmentCargos.GetElement( index - 1 - cargo_start_index ).MoveGridCursor( direction );
				}
			}
			else
			{
				m_AttachmentCargos.GetElement( index - cargo_start_index ).MoveGridCursor( direction );
			}
		}
		ScrollToActiveContainer( GetFocusedContainer() );
	}
	
	void DestroyCargo()
	{
		if( m_CargoGrid )
		{
			m_Body.RemoveItem( m_CargoGrid );
			delete m_CargoGrid;
		}
		
		if( m_AttachmentCargos )
		{
			foreach( CargoContainer cargo : m_AttachmentCargos )
			{
				m_Body.RemoveItem( cargo );
				delete cargo;
			}
		}

		m_AttachmentCargos.Clear();
		Refresh();
		m_Parent.Refresh();
	}
	
	void DropReceived( Widget w,  int x, int y, CargoContainer cargo )
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
		
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}

		int idx = 0;
		int c_x, c_y;
		
		EntityAI target_entity;
		CargoBase target_cargo;
		
		if( cargo == m_CargoGrid )
		{
			target_entity	= m_Entity;
			target_cargo 	= m_Entity.GetInventory().GetCargo();
		}
		else
		{
			target_entity	= m_AttachmentCargos.GetKeyByValue( cargo );
			if( target_entity )
			{
				target_cargo 	= target_entity.GetInventory().GetCargo();
			}
			else
				return;
		}
		
		if( target_cargo && target_entity )
		{
			c_x = target_cargo.GetHeight();
			c_y = target_cargo.GetWidth();
		}
		else
			return;
		
		if( c_x > x && c_y > y && target_entity.GetInventory().CanAddEntityInCargoEx( item, idx, x, y ) )
		{
			TakeIntoCargo( player, target_entity, item, idx, x, y );

			Icon icon = cargo.GetIcon( item );
			
			if( icon )
			{
				w.FindAnyWidget("Selected").SetColor( ColorManager.BASE_COLOR );
				icon.Refresh();
				Refresh();
			}
		}
	}
	
	bool DraggingOverGrid( Widget w,  int x, int y, Widget reciever, CargoContainer cargo )
	{
		EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
		if( w == null )
		{
			return false;
		}
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return false;
		}

		int color;
		int idx = 0;
		int c_x, c_y;
		
		if( item_in_hands.GetInventory().GetCargo() )
		{
			c_x = item_in_hands.GetInventory().GetCargo().GetHeight();
			c_y = item_in_hands.GetInventory().GetCargo().GetWidth();
		}
		
		if( c_x > x && c_y > y && item_in_hands.GetInventory().CanAddEntityInCargoEx( item, 0, x, y ) )
		{
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
			color = ColorManager.GREEN_COLOR;
		}
		else
		{
			color = ColorManager.RED_COLOR;
			ItemManager.GetInstance().ShowSourceDropzone( item );
		}

		if( w.FindAnyWidget("Selected") )
		{
			w.FindAnyWidget("Selected").SetColor( color );
		}
		else
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Selected" );
			w.FindAnyWidget( name ).SetColor( color );
		}

		return true;
	}
	
	void TakeIntoCargo( notnull PlayerBase player, notnull EntityAI entity, notnull EntityAI item, int idx = -1, int row = 0, int col = 0 )
	{
		ItemBase item_base = ItemBase.Cast( item );
		float stackable = item_base.ConfigGetFloat("varStackMax");
		
		if( !item.GetInventory().CanRemoveEntity() || !player.CanManipulateInventory() )
			return;
		
		if( stackable == 0 || stackable >= item_base.GetQuantity() )
		{
			if( idx != -1 )
				player.PredictiveTakeEntityToTargetCargoEx( entity, item, idx, row, col );
			else
				player.PredictiveTakeEntityToTargetAttachment(entity, item);
		}
		else if( stackable != 0 && stackable < item_base.GetQuantity() )
		{
			item_base.SplitIntoStackMaxCargoClient( entity, idx, row, col );
		}
	}
	
	void TakeAsAttachment( Widget w, Widget receiver )
	{
		EntityAI receiver_item;
		int slot_id = -1;
		string name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name) );
		if( receiver_iw )
		{
			receiver_item	= receiver_iw.GetItem();
			slot_id			= receiver_iw.GetUserID();
		}
		
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( !item.GetInventory().CanRemoveEntity() || !player.CanManipulateInventory() )
			return;
		
		EntityAI target_att_entity = m_Entity;
		Weapon_Base wpn;
		Magazine mag;
		if( Class.CastTo(wpn,  m_Entity ) && Class.CastTo(mag,  item ) )
		{
			if( player.GetWeaponManager().CanAttachMagazine( wpn, mag ) )
			{
				player.GetWeaponManager().AttachMagazine( mag );
			}
		}
		else if( receiver_item )
		{
			if( ( ItemBase.Cast( receiver_item ) ).CanBeCombined( ItemBase.Cast( item ) ) )
			{
				( ItemBase.Cast( receiver_item ) ).CombineItemsClient( ItemBase.Cast( item ) );
			}
			else if( GameInventory.CanSwapEntities( receiver_item, item ) )
			{
				if( !receiver_item.GetInventory().CanRemoveEntity() )
					return;
				GetGame().GetPlayer().PredictiveSwapEntities( item, receiver_item );
			}
		}
		else if( m_Entity.GetInventory().CanAddAttachment( item ) )
		{
			ItemBase item_base	= ItemBase.Cast( item );
			float stackable		= item_base.ConfigGetFloat("varStackMax");
			
			if( stackable == 0 || stackable >= item_base.GetQuantity() )
			{
				if( slot_id != -1 )
					player.PredictiveTakeEntityToTargetAttachmentEx(m_Entity, item, slot_id);
				else
					player.PredictiveTakeEntityToTargetAttachment(m_Entity, item);
			}
			else if( stackable != 0 && stackable < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( m_Entity, slot_id );
			}
		}
		else if( ( m_Entity.GetInventory().CanAddEntityInCargo( item ) && ( !player.GetInventory().HasEntityInInventory( item ) || !m_Entity.GetInventory().HasEntityInCargo( item )) ) || player.GetHumanInventory().HasEntityInHands( item ) )
		{
			TakeIntoCargo( PlayerBase.Cast( GetGame().GetPlayer() ), m_Entity, item );
		}
		else if( !player.GetInventory().HasEntityInInventory( item ) || !m_Entity.GetInventory().HasEntityInCargo( item ) )
		{
			TakeIntoCargo( PlayerBase.Cast( GetGame().GetPlayer() ), m_Entity, item );
		}
		
		ItemManager.GetInstance().HideDropzones();
	}

	void ShowAtt( EntityAI entity )
	{
		m_Entity = entity;
		m_ActiveIndex = 1;
		if( entity.GetSlotsCountCorrect() > 0 )
		{
			m_Atts = new Attachments( this, entity );
			m_Atts.InitAttachmentGrid( 1 );
		}
		
		if( entity.GetInventory().GetCargo() )
		{
			m_CargoGrid = new CargoContainer( this );
			m_CargoGrid.SetEntity( entity );
			Insert( m_CargoGrid );
		}

		if( entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) )
		{
			HideContent();
		}
		else
		{
			ShowContent();
		}
		
		m_CollapsibleHeader.ShowCollapseButtons( m_Atts || m_CargoGrid, m_Atts || m_CargoGrid );

		Refresh();
		m_Parent.Refresh();
	}
	
	void RefreshHands()
	{
		for ( int i = 1; i < m_Body.Count(); i++ )
		{
			m_Body.Get( i ).OnShow();
		}
		Refresh();
		m_Parent.Refresh();
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();
		
		if( m_Entity )
		{
			if( m_Entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) || m_Hidden )
			{
				HideContent();
			}
			else
			{
				ShowContent();
				if( m_Atts )
				{
					m_Atts.RefreshAtt();
				}
				if( m_CargoGrid )
				{
					m_CargoGrid.UpdateInterval();
				}
				
				EntityAI e;
				int i;
				array<EntityAI> cargo_attachments = m_Entity.GetAttachmentsWithCargo();
				if( cargo_attachments && m_AttachmentCargos )
				{
					for( i = 0; i < m_AttachmentCargos.Count(); i++ )
					{
						e			= m_AttachmentCargos.GetKey( i );
						if( e )
						{
							if( cargo_attachments.Find( e ) == -1 )
							{
								CargoContainer old_cont = m_AttachmentCargos.GetElement( i );
								m_Body.RemoveItem( old_cont );
								m_AttachmentCargos.Remove( e );
								
								delete old_cont;
								
								SetFocusToIndex();
								
								Refresh();
								m_Parent.Refresh();
							}
							else
							{
								m_AttachmentCargos.Get( e ).UpdateInterval();
							}
						}
					}
					
					for( i = 0; i < cargo_attachments.Count(); i++ )
					{
						if( !m_AttachmentCargos.Contains( cargo_attachments.Get( i ) ) )
						{
							ref CargoContainer cont = new CargoContainer( this, true );
							cont.GetRootWidget().SetSort( i + 50 );
							cont.SetEntity( cargo_attachments.Get( i ) );
							Insert( cont, m_Atts.GetAttachmentHeight() + 1 + m_AttachmentCargos.Count() );
							
							m_AttachmentCargos.Insert( cargo_attachments.Get( i ), cont );
							Refresh();
							m_Parent.Refresh();
						}
					}
				}
				
				array<EntityAI> attachment_attachments = m_Entity.GetAttachmentsWithAttachments();
				if( attachment_attachments && m_AttachmentAttachments )
				{
					for( i = 0; i < m_AttachmentAttachmentsContainers.Count(); i++ )
					{
						e			= m_AttachmentAttachmentsContainers.GetKey( i );
						if( e )
						{
							if( attachment_attachments.Find( e ) == -1 )
							{
								AttachmentsWrapper old_att_cont = m_AttachmentAttachmentsContainers.GetElement( i );
								m_Body.RemoveItem( old_att_cont );
								m_AttachmentAttachmentsContainers.Remove( e );
								m_AttachmentAttachments.Remove( e );
								
								m_Body.RemoveItem( old_att_cont );
								m_AttachmentAttachments.Remove( e );
								delete old_att_cont;
								
								SetFocusToIndex();
								Refresh();
								m_Parent.Refresh();
							}
							else
							{
								m_AttachmentAttachments.Get( e ).RefreshAtt();
							}
						}
					}
					
					for( i = 0; i < attachment_attachments.Count(); i++ )
					{
						if( !m_AttachmentAttachments.Contains( attachment_attachments.Get( i ) ) )
						{
							ref Attachments att_cont = new Attachments( this, attachment_attachments.Get( i ) );
							att_cont.InitAttachmentGrid( i + m_Atts.GetAttachmentHeight() + 1 );
							
							m_AttachmentAttachments.Insert( attachment_attachments.Get( i ), att_cont );
							m_AttachmentAttachmentsContainers.Insert( attachment_attachments.Get( i ), att_cont.GetWrapper() );
							
							att_cont.RefreshAtt();
							
							Refresh();
							m_Parent.Refresh();
						}
					}
				}
			}
		}
		
		ElectricityIcon();
		m_CollapsibleHeader.UpdateInterval();
	}

	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.ClosableContainer;
	}

	override void OnShow()
	{
		super.OnShow();

		for ( int i = 0; i < m_Body.Count(); i++ )
		{
			m_Body.Get( i ).OnShow();
		}
	}

	void DoubleClick(Widget w, int x, int y, int button)
	{
		if( button == MouseState.LEFT )
		{
			if( w == null )
			{
				return;
			}
			ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
			if( !iw )
			{
			  string name = w.GetName();
			  name.Replace( "PanelWidget", "Render" );
			  iw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
			}
			if( !iw )
			{
			  iw = ItemPreviewWidget.Cast( w );
			}
			if( !iw.GetItem() )
			{
				return;
			}
	
			ItemBase item = ItemBase.Cast( iw.GetItem() );
			
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
			Weapon_Base wpn;
			Magazine mag;
			if( Class.CastTo(wpn, item_in_hands ) )
			{
				return;
			}
	
			if( !item.GetInventory().CanRemoveEntity() )
				return;
			
			if( player.GetInventory().HasEntityInInventory( item ) && player.GetHumanInventory().CanAddEntityInHands( item ) )
			{
				player.PredictiveTakeEntityToHands( item );
			}
			else
			{
				if( player.GetInventory().CanAddEntityToInventory( item ) && player.GetHumanInventory().CanRemoveEntityInHands() )
				{
					if( item.ConfigGetFloat("varStackMax") )
						item.SplitIntoStackMaxClient( player, -1, );
					else
						player.PredictiveTakeEntityToInventory( FindInventoryLocationType.ANY, InventoryItem.Cast( item ) );
				}
				else
				{
					if( player.GetHumanInventory().CanAddEntityInHands( item ) )
					{
						player.PredictiveTakeEntityToHands( item );
					}
				}
			}
	
			ItemManager.GetInstance().HideTooltip();
	
			name = w.GetName();
			name.Replace( "PanelWidget", "Temperature" );
			w.FindAnyWidget( name ).Show( false );
		}
	}
	
	void ElectricityIcon()
	{
		EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
		if( item_in_hands && item_in_hands.GetCompEM() )
		{
			if( GetRootWidget().FindAnyWidget( "electricity" ) )
			{
				bool show_electricity_icon = item_in_hands.GetCompEM().HasElectricityIcon();
				GetRootWidget().FindAnyWidget( "electricity" ).Show( show_electricity_icon );
			}
			
			if( GetRootWidget().FindAnyWidget( "electricity_can_work" ) )
			{
				bool show_electricity_can_work_icon = item_in_hands.GetCompEM().CanWork() && !item_in_hands.IsRuined();
				GetRootWidget().FindAnyWidget( "electricity_can_work" ).Show( show_electricity_can_work_icon );
			}
		}
		else
		{
			GetRootWidget().FindAnyWidget( "electricity" ).Show( false );
		}
	}

	void CollapseButtonOnMouseButtonDown( Widget w )
	{
		if( !m_Hidden )
		{
			for ( int i = 1; i < m_Body.Count(); i++ )
			{
				m_Body.Get( i ).OnHide();
			}
			m_Hidden = true;
		}
		else
		{
			for ( i = 1; i < m_Body.Count(); i++ )
			{
				m_Body.Get( i ).OnShow();
			}
			m_Hidden = false;
		}

		w.FindAnyWidget("opened").Show(!m_Hidden);
		w.FindAnyWidget("closed").Show(m_Hidden);
		
		m_Parent.Refresh();
	}
}
