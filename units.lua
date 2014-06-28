require "tundra.syntax.glob"

Program {
	Name = "JiNXGL",
	Sources = { Glob { Dir = "source", Extensions = { ".cpp" } } },
	Frameworks = { "Cocoa", "OpenGL", "IOKit", "CoreVideo"  },
	ReplaceEnv = { LD = { "$(CXX)" ; Config = { "*-clang-*" } }, },
	Libs = {
	    { "GLEW", "glfw3" },
	}

}

Default "JiNXGL"