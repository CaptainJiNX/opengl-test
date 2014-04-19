require "tundra.syntax.glob"

Program {
	Name = "JiNXGL",
	Sources = { Glob { Dir = "src", Extensions = { ".cpp" } } },
	ReplaceEnv = { LD = { "$(CXX)" ; Config = { "*-clang-*" } }, },
}

Default "JiNXGL"