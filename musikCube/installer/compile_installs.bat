SET PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem;C:\progra~1\NSIS
makensis musikCubeSetup.nsi
makensis /DCOMPILE_APE musikCubePlugins.nsi
makensis /DCOMPILE_FLAC musikCubePlugins.nsi
makensis /DCOMPILE_M4A musikCubePlugins.nsi
makensis /DCOMPILE_MOD musikCubePlugins.nsi
makensis /DCOMPILE_WAV musikCubePlugins.nsi
makensis /DCOMPILE_WMA musikCubePlugins.nsi
makensis /DCOMPILE_MPC musikCubePlugins.nsi
makensis /DCOMPILE_WV musikCubePlugins.nsi