
//! BiosCheckUpdateResult represent result of the PromptUpdateAsync request.
class BiosCheckUpdateResult
{
	//! Is new update available?
	bool	m_IsUpdate;
	
	//! Is update mandatory?
	bool	m_IsMandatory;
};

class BiosPackageService
{
	//! Async check if exist new update
	/*!
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError CheckUpdateAsync();
	
	
	//! Prompt user to accept update with system GUI
	/*!
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError PromptUpdateAsync();
	
	
	
	//! Callback function for CheckUpdateAsync()
	/*!
		@param checkUpdateResult contain information about availability of new update and if it is mandatory.
		@param error indicating success or fail of the async operation.
	*/
	void OnCheckUpdate(BiosCheckUpdateResult checkUpdateResult, EBiosError error)
	{
		
	}
	
	
	//! Callback function for PromptUpdateAsync()
	/*!
		Show system UI with update.
		On Xbox, game suspend after accept update.

		@param error indicating success or fail of the async operation.
	*/
	void OnPromptUpdate(EBiosError error)
	{
		OnlineServices.ErrorCaught( error );
	}
};