require "tundra.syntax.glob"

Program {
	Name = "JiNXGL",
	Sources = { Glob { Dir = "src", Extensions = { ".cpp" } } },
	Frameworks = { "Cocoa", "OpenGL", "IOKit", "CoreVideo"; Config = { "macosx-*-*"  } },
	ReplaceEnv = { LD = { "$(CXX)" ; Config = { "*-clang-*" } }, },
	Libs = {
		{ "opengl32.lib", "glew32.lib", "glfw3dll.lib"; Config = { "win32-*-*", "win64-*-*" } } ,
	    { "GLEW", "glfw3"; Config = { "macosx-*-*"  }} ,
	}

}

Default "JiNXGL"