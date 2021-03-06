///@{ events

/**@enum	HandEventID
 * @brief	identifier for events mainly for rpc purposes
 **/
enum HandEventID
{
	UNKNOWN,
	TAKE,
	MOVETO,
	DROP,
	THROW,
	SWAP,
	FORCESWAP,
	DESTROY,
	CREATED,
	DESTROYED,
	REPLACE,
	REPLACE2,
	REPLACE3,
	REPLACED,
	HUMANCOMMAND_ACTION_FINISHED,
	HUMANCOMMAND_ACTION_ABORTED,
	ANIMEVENT_CHANGE_HIDE
};

enum JunctureRequestResult
{
	JUNCTURE_NOT_REQUIRED,
	JUNCTURE_ACQUIRED,
	JUNCTURE_DENIED,
	ERROR,
}

/**@class	HandEventBase
 * @brief	represents event that triggers transition from state to state
 **/
class HandEventBase
{
	int m_EventID = 0;
	int m_AnimationID = -1;
	Man m_Player;
	EntityAI m_Entity;

	void HandEventBase (Man p = null, EntityAI e = NULL) { m_Player = p; m_Entity = e; }
	HandEventID GetEventID () { return m_EventID; }

	void ReadFromContext (ParamsReadContext ctx) { }
	void WriteToContext (ParamsWriteContext ctx)
	{
		ctx.Write(m_EventID);
		ctx.Write(m_Player);
		ctx.Write(m_Entity);
		ctx.Write(m_AnimationID);
	}

	InventoryLocation GetDst () { return null; }
	int GetAnimationID () { return m_AnimationID; }
	bool AcquireInventoryJunctureFromServer (notnull Man player) { return false; }
	bool CheckRequest () { return true; }
	bool IsServerSideOnly () { return false; }

	static HandEventBase HandEventFactory (HandEventID id, Man p = null, EntityAI e = NULL)
	{
		switch (id)
		{
			case HandEventID.UNKNOWN: return null;
			case HandEventID.TAKE: return new HandEventTake(p, e);
			case HandEventID.MOVETO: return new HandEventMoveTo(p, e);
			case HandEventID.DROP: return new HandEventDrop(p, e);
			case HandEventID.THROW: return new HandEventThrow(p, e);
			case HandEventID.SWAP: return new HandEventSwap(p, e);
			case HandEventID.FORCESWAP: return new HandEventForceSwap(p, e);
			case HandEventID.DESTROY: return new HandEventDestroy(p, e);
			case HandEventID.CREATED: return new HandEventCreated(p, e);
			case HandEventID.DESTROYED: return new HandEventDestroyed(p, e);
			case HandEventID.REPLACE: return new HandEventDestroyAndReplaceWithNew(p, e);
			case HandEventID.REPLACE2: return new HandEventDestroyAndReplaceWithNewElsewhere(p, e);
			case HandEventID.REPLACE3: return new HandEventDestroyElsewhereAndReplaceWithNewInHands(p, e);
			case HandEventID.REPLACED: return new HandEventReplaced(p, e);
			case HandEventID.ANIMEVENT_CHANGE_HIDE: return HandAnimEventChanged(p, e);
			case HandEventID.HUMANCOMMAND_ACTION_FINISHED : return HandEventHumanCommandActionFinished(p, e);
			case HandEventID.HUMANCOMMAND_ACTION_ABORTED : return HandEventHumanCommandActionAborted(p, e);
		}
		Error("[hndfsm] HandEventFactory - unregistered hand event with id=" + id);
		return null;
	}
	
	static HandEventBase CreateHandEventFromContext (ParamsReadContext ctx)
	{
		int eventID = -1;
		if (!ctx.Read(eventID))
		{
			Error("[hndfsm] CreateHandEventFromContext - cannot read eventID");
			return null;
		}
		Man player;
		if (!ctx.Read(player))
		{
			Error("[hndfsm] CreateHandEventFromContext - cannot read player");
			return null;
		}		
		EntityAI entity;
		if (!ctx.Read(entity))
		{
			Error("[hndfsm] CreateHandEventFromContext - cannot read entity");
			return null;
		}
		int animID = -1;
		if (!ctx.Read(animID))
		{
			Error("[hndfsm] CreateHandEventFromContext - cannot read animID");
			return null;
		}
		HandEventBase b = HandEventFactory(eventID, player, entity);
		if (b)
		{
			b.m_AnimationID = animID;
			b.ReadFromContext(ctx);
		}
		return b;
	}
	
	string DumpToString ()
	{
		string res = "{ HandEv id=" + typename.EnumToString(HandEventID, GetEventID());
		res = res + " pl=" + m_Player + " e= " + m_Entity;
		res = res + " }";
		return res;
	}
};

class HandEventTake extends HandEventBase
{
	void HandEventTake (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.TAKE; }

	override InventoryLocation GetDst ()
	{
		InventoryLocation dst = new InventoryLocation;
		dst.SetHands(m_Player, m_Entity);
		return dst;
	}
	
	override bool CheckRequest ()
	{
		return GameInventory.CheckMoveToDstRequest(m_Player, m_Entity, GetDst(), GameInventory.c_MaxItemDistanceRadius);
	}

	override bool AcquireInventoryJunctureFromServer (notnull Man player)
	{
		InventoryLocation src = new InventoryLocation;
		if (m_Entity.GetInventory().GetCurrentInventoryLocation(src))
		{
			InventoryLocation dst = GetDst();
			return TryAcquireInventoryJunctureFromServer(player, src, dst);
		}
		Error("[hndfsm] HandEventTake. AcquireInventoryJunctureFromServer: no inv loc for entity=" + m_Entity.GetName() + "@" + m_Entity + " ev=" + DumpToString());
		return JunctureRequestResult.ERROR;
	}
};

class HandEventMoveTo extends HandEventBase
{
	ref InventoryLocation m_Dst; /// destination for item in hands

	void HandEventMoveTo (Man p = null, EntityAI e = NULL, InventoryLocation dst = NULL) { m_EventID = HandEventID.MOVETO; m_Dst = dst; }

	override void ReadFromContext (ParamsReadContext ctx)
	{
		m_Dst = new InventoryLocation;
		super.ReadFromContext(ctx);
		m_Dst.ReadFromContext(ctx);
	}
	override void WriteToContext (ParamsWriteContext ctx)
	{
		super.WriteToContext(ctx);
		m_Dst.WriteToContext(ctx);
	}

	override InventoryLocation GetDst () { return m_Dst; }
	
	override bool CheckRequest ()
	{
		return GameInventory.CheckMoveToDstRequest(m_Player, m_Entity, GetDst(), GameInventory.c_MaxItemDistanceRadius);
	}

	override bool AcquireInventoryJunctureFromServer (notnull Man player)
	{
		InventoryLocation src = new InventoryLocation;
		if (m_Entity.GetInventory().GetCurrentInventoryLocation(src))
		{
			InventoryLocation dst = GetDst();
			return TryAcquireInventoryJunctureFromServer(player, src, dst);
		}
		Error("[hndfsm] HandEventMoveTo.AcquireInventoryJunctureFromServer: no inv loc for entity=" + m_Entity.GetName() + "@" + m_Entity + " ev=" + DumpToString());
		return JunctureRequestResult.ERROR;
	}
};

class HandEventDrop extends HandEventBase
{
	void HandEventDrop (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.DROP; }

	override InventoryLocation GetDst ()
	{
		InventoryLocation dst = new InventoryLocation;
		vector mat[4];
		Math3D.MatrixIdentity4(mat);
		mat[3] = m_Player.GetPosition();

		dst.SetGround(m_Entity, mat);
		return dst;
	}
	
	override bool CheckRequest ()
	{
		return GameInventory.CheckMoveToDstRequest(m_Player, m_Entity, GetDst(), GameInventory.c_MaxItemDistanceRadius);
	}

	override bool AcquireInventoryJunctureFromServer (notnull Man player)
	{
		InventoryLocation src = new InventoryLocation;
		if (m_Entity.GetInventory().GetCurrentInventoryLocation(src))
		{
			InventoryLocation dst = GetDst();
			return TryAcquireInventoryJunctureFromServer(player, src, dst);
		}
		Error("[hndfsm] HandEventDrop.AcquireInventoryJunctureFromServer: no inv loc for entity=" + m_Entity.GetName() + "@" + m_Entity + " ev=" + DumpToString());
		return JunctureRequestResult.ERROR;
	}
};

class HandEventThrow extends HandEventBase
{
	void HandEventThrow (Man p = null, EntityAI e = null) 
	{ 
		m_EventID = HandEventID.THROW; 
	}

/*	override InventoryLocation GetDst ()
	{
		vector mat[4];
		m_Entity.GetTransform(mat);

		InventoryLocation dst = new InventoryLocation;
		dst.SetGround(m_Entity, mat);
		return dst;
	}
	
	override bool CheckRequest ()
	{
		return GameInventory.CheckMoveToDstRequest(m_Player, m_Entity, GetDst(), GameInventory.c_MaxItemDistanceRadius);
	}

	override bool AcquireInventoryJunctureFromServer (notnull Man player)
	{
		InventoryLocation src = new InventoryLocation;
		if (m_Entity.GetInventory().GetCurrentInventoryLocation(src))
		{
			InventoryLocation dst = GetDst();
			return TryAcquireInventoryJunctureFromServer(player, src, dst);
		}
		Error("[hndfsm] HandEventDrop.AcquireInventoryJunctureFromServer: no inv loc for entity=" + m_Entity.GetName() + "@" + m_Entity + " ev=" + DumpToString());
		return JunctureRequestResult.ERROR;
	}*/
};

class HandEventSwap extends HandEventBase
{
	int m_Animation2ID = -1;

	void HandEventSwap (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.SWAP; }
	
	override void ReadFromContext (ParamsReadContext ctx)
	{
		super.ReadFromContext(ctx);
		ctx.Read(m_Animation2ID);
	}

	override void WriteToContext (ParamsWriteContext ctx)
	{
		super.WriteToContext(ctx);
		ctx.Write(m_Animation2ID);
	}

	override InventoryLocation GetDst ()
	{
		EntityAI inHands = m_Player.GetHumanInventory().GetEntityInHands();

		InventoryLocation new_src = new InventoryLocation;
		InventoryLocation old_src = new InventoryLocation;
		if (inHands && m_Entity && inHands && m_Entity.GetInventory().GetCurrentInventoryLocation(new_src) && inHands.GetInventory().GetCurrentInventoryLocation(old_src))
		{
			InventoryLocation old_dst = new InventoryLocation;
			old_dst.Copy(old_src);
			old_dst.CopyLocationFrom(new_src);
			return old_dst;
		}
		Error("[hndfsm] HandEventSwap - cannot create Dst");
		return null;
	}
	
	override bool CheckRequest ()
	{
		EntityAI inHands = m_Player.GetHumanInventory().GetEntityInHands();
		if (m_Entity && inHands)
		{
			bool test1 = GameInventory.CheckSwapItemsRequest(m_Player, m_Entity, inHands, GameInventory.c_MaxItemDistanceRadius);
			if (!test1)
				Print("Warning: test1 failed");
			bool test2 = GameInventory.CanSwapEntities(m_Entity, inHands);
			if (!test2)
				Print("Warning: test2 failed");
			return test1 && test2;
		}
		return false;
	}

	override bool AcquireInventoryJunctureFromServer (notnull Man player)
	{
		EntityAI item1 = m_Entity;
		EntityAI item2 = m_Player.GetHumanInventory().GetEntityInHands();

		if (item1 && item2)
		{
			InventoryLocation src1, src2, dst1, dst2;
			if (GameInventory.MakeSrcAndDstForSwap(item1, item2, src1, src2, dst1, dst2))
			{
				return TryAcquireTwoInventoryJuncturesFromServer(player, src1, src2, dst1, dst2);
			}
			else
			{
				Error("[hndfsm] HandEventSwap.AcquireInventoryJunctureFromServer: GameInventory.MakeSrcAndDstForSwap failed");
				return JunctureRequestResult.ERROR;
			}
		}

		Error("[hndfsm] HandEventSwap.AcquireInventoryJunctureFromServer: one of items is null: item1=" + item1 + " or item2=" + item2);
		return JunctureRequestResult.ERROR;
	}
};

class HandEventForceSwap extends HandEventBase
{
	ref InventoryLocation m_Dst; /// destination of item in hands
	int m_Animation2ID = -1;

	void HandEventForceSwap (Man p = null, EntityAI e = NULL, InventoryLocation dst = NULL) { m_EventID = HandEventID.FORCESWAP; m_Dst = dst; }

	override void ReadFromContext (ParamsReadContext ctx)
	{
		m_Dst = new InventoryLocation;
		super.ReadFromContext(ctx);
		m_Dst.ReadFromContext(ctx);
		ctx.Read(m_Animation2ID);
	}
	override void WriteToContext (ParamsWriteContext ctx)
	{
		super.WriteToContext(ctx);
		m_Dst.WriteToContext(ctx);
		ctx.Write(m_Animation2ID);
	}

	override InventoryLocation GetDst () { return m_Dst; }
	
	override bool CheckRequest ()
	{
		EntityAI inHands = m_Player.GetHumanInventory().GetEntityInHands();
		if (m_Entity && inHands && m_Dst && m_Dst.IsValid())
		{
			bool test1 = GameInventory.CheckSwapItemsRequest(m_Player, m_Entity, inHands, GameInventory.c_MaxItemDistanceRadius);
			bool test2 = GameInventory.CanForceSwapEntities(m_Entity, inHands, m_Dst);
			return test1 && test2;
		}
		return false;
	}

	override bool AcquireInventoryJunctureFromServer (notnull Man player)
	{
		EntityAI item1 = m_Entity;
		EntityAI item2 = m_Player.GetHumanInventory().GetEntityInHands();
		InventoryLocation user_dst2 = m_Dst;

		if (item1 && item2 && user_dst2)
		{
			InventoryLocation src1, src2, dst1;
			if (GameInventory.MakeSrcAndDstForForceSwap(item1, item2, src1, src2, dst1, user_dst2))
			{
				return TryAcquireTwoInventoryJuncturesFromServer(player, src1, src2, dst1, user_dst2);
			}
			else
			{
				Error("[hndfsm] HandEventForceSwap.AcquireInventoryJunctureFromServer: GameInventory.MakeSrcAndDstForForceSwap failed");
				return JunctureRequestResult.ERROR;
			}
		}

		Error("[hndfsm] HandEventForceSwap.AcquireInventoryJunctureFromServer: one of items is null: item1=" + item1 + " or item2=" + item2 + " or user_dst2=" + user_dst2);
		return JunctureRequestResult.ERROR;
	}
};

class HandEventDestroy extends HandEventBase
{
	void HandEventDestroy (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.DESTROY; }
	
	override bool IsServerSideOnly () { return true; }
};

class HandEventCreated extends HandEventBase
{
	void HandEventCreated (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.CREATED; }
};

class HandEventDestroyed extends HandEventBase
{
	void HandEventDestroyed (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.DESTROYED; }
};

class HandEvenReplaceWithNewBase extends HandEventBase
{
	string m_Type;
	ref ReplaceItemWithNewLambdaBase m_Lambda;

	void HandEvenReplaceWithNewBase (Man p = null, EntityAI e = NULL, ReplaceItemWithNewLambdaBase lambda = NULL) { m_EventID = HandEventID.REPLACE; m_Lambda = lambda; }

	override void ReadFromContext (ParamsReadContext ctx)
	{
		super.ReadFromContext(ctx);
		ctx.Read(m_Type);
		Error("[hndfsm] HandEventDestroyAndReplaceWithNew - Cannot serialize lambda (read)");
	}
	override void WriteToContext (ParamsWriteContext ctx)
	{
		super.WriteToContext(ctx);
		ctx.Write(m_Type);
		Error("[hndfsm] HandEventDestroyAndReplaceWithNew - Cannot serialize lambda (write)");
	}

	override bool IsServerSideOnly () { return true; }
	
	override InventoryLocation GetDst ()
	{
		InventoryLocation dst = new InventoryLocation;
		dst.SetHands(m_Player, m_Entity);
		return dst;
	}
};

class HandEventDestroyAndReplaceWithNew extends HandEvenReplaceWithNewBase
{
	void HandEventDestroyAndReplaceWithNew (Man p = null, EntityAI e = NULL, ReplaceItemWithNewLambdaBase lambda = NULL) { m_EventID = HandEventID.REPLACE; m_Lambda = lambda; }
};

class HandEventDestroyAndReplaceWithNewElsewhere extends HandEventDestroyAndReplaceWithNew
{
	void HandEventDestroyAndReplaceWithNewElsewhere (Man p = null, EntityAI e = NULL, ReplaceItemWithNewLambdaBase lambda = NULL) { m_EventID = HandEventID.REPLACE3; m_Lambda = lambda; }
};

class HandEventDestroyElsewhereAndReplaceWithNewInHands extends HandEvenReplaceWithNewBase
{
	void HandEventDestroyElsewhereAndReplaceWithNewInHands (Man p = null, EntityAI e = NULL, ReplaceItemWithNewLambdaBase lambda = NULL) { m_EventID = HandEventID.REPLACE3; m_Lambda = lambda; }
};

class HandEventReplaced extends HandEventBase
{
	void HandEventReplaced (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.REPLACED; }
};

// anim events

class HandAnimEventChanged extends HandEventBase
{
	void HandAnimEventChanged (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.ANIMEVENT_CHANGE_HIDE; }
};

HandEventBase HandAnimEventFactory (WeaponEvents type, Man p = null, EntityAI e = NULL)
{
	switch (type)
	{
		case WeaponEvents.CHANGE_HIDE: return new HandAnimEventChanged(p, e);
	}
	return NULL;
}

/**@brief		triggered when animation action finishes
 **/
class HandEventHumanCommandActionFinished extends HandEventBase
{
	void HandEventHumanCommandActionFinished (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.HUMANCOMMAND_ACTION_FINISHED; }
};
/**@brief		triggered when animation action aborts
 **/
class HandEventHumanCommandActionAborted extends HandEventBase
{
	void HandEventHumanCommandActionAborted (Man p = null, EntityAI e = NULL) { m_EventID = HandEventID.HUMANCOMMAND_ACTION_ABORTED; }
};

///@} events

