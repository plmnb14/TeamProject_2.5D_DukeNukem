// 명령어			옵션			복사 경로									붙여넣기 경로
xcopy			/y			.\Engine\Headers\*.h					.\Reference\Headers\
xcopy			/y			.\Engine\Exports\*.*					.\Reference\Headers\

xcopy			/y			.\Engine\System\Codes\*.h				.\Reference\Headers\
xcopy			/y			.\Engine\System\Bin\System.lib			.\Reference\Libraries\
xcopy			/y			.\Engine\System\Bin\System.dll			.\Client\Bin\
xcopy			/y			.\Engine\System\Bin\System.dll			.\Tool\

xcopy			/y			.\Engine\Utilities\Codes\*.h			.\Reference\Headers\
xcopy			/y			.\Engine\Utilities\Bin\Utilities.lib	.\Reference\Libraries\
xcopy			/y			.\Engine\Utilities\Bin\Utilities.dll	.\Client\Bin\
xcopy			/y			.\Engine\Utilities\Bin\Utilities.dll	.\Tool\

xcopy			/y			.\Engine\Resources\Codes\*.h			.\Reference\Headers\
xcopy			/y			.\Engine\Resources\Bin\Resources.lib	.\Reference\Libraries\
xcopy			/y			.\Engine\Resources\Bin\Resources.dll	.\Client\Bin\
xcopy			/y			.\Engine\Resources\Bin\Resources.dll	.\Tool\

