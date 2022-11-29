add_library(winhttp SHARED)
target_sources(winhttp PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)