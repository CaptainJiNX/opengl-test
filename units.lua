require "tundra.syntax.glob"

Program {
	Name = "JiNXGL",
	Sources = { Glob { Dir = "src", Extensions = { ".cpp" } } },
	Frameworks = { "Cocoa", "OpenGL", "IOKit", "CoreVideo"  },
	ReplaceEnv = { LD = { "$(CXX)" ; Config = { "*-clang-*" } }, },
	Libs = {
	    { "GLEW", "glfw3"},
	}

}

Default "JiNXGL"