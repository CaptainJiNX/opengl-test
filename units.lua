require "tundra.syntax.glob"

Program {
	Name = "JiNXGL",
	Sources = {
		Glob {
			Dir = "src",
			Extensions = { ".cpp" }
		}
	},
}

Default "JiNXGL"