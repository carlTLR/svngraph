project(binscmgraph)

set( binscmgraph_LIBS RepositoryFactory SVNRepository )
INCLUDE_DIRECTORIES(${scmgraph_SOURCE_DIR}/lib)
LINK_LIBRARIES( ${binscmgraph_LIBS} )

set( binscmgraph_SRCS main )

ADD_EXECUTABLE(scmGraph ${binscmgraph_SRCS})
