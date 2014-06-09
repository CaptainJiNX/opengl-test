Build {
	Units = "units.lua",
	Configs = {
		{
			Name = "macosx-clang",
			DefaultOnHost = "macosx",
			Tools = { "clang-osx" },
			Env = { CXXOPTS_DEBUG = "-g" },
		},
		{
			Name = "win32-msvc",
			DefaultOnHost = "windows",
			Tools = { "msvc-vs2012" },
			Env = {
	        	CXXOPTS = { "/EHsc" },
			},
		},
	},
}
