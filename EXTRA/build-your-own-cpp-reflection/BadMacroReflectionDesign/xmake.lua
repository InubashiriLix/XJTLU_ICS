add_rules("plugin.compile_commands.autoupdate", { outputdir = "build/" })

target("reflection")
set_kind("binary")
set_languages("c++20")
add_files("src/*.cc")
add_includedirs("inc/")
add_links("m")
target_end()
