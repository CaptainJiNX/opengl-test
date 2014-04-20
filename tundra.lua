Build {
	Units = "units.lua",
	Configs = {
		{
			Name = "macosx-clang",
			DefaultOnHost = "macosx",
			Tools = { "clang-osx" },
			Env = { CXXOPTS_DEBUG = "-g" },

		},
	},
}
