project(libscmgraph)

INCLUDE_DIRECTORIES( /usr/include/apr-1 /usr/include/subversion-1 )
LINK_LIBRARIES( svn_client-1 )
LINK_LIBRARIES( boost_regex )

set( libscmgraph_SRCS RepositoryFactory SVNRepository )
foreach(next_ITEM ${libscmgraph_SRCS})
  ADD_LIBRARY( ${next_ITEM} SHARED ${next_ITEM} )
endforeach(next_ITEM ${libscmgraph_SRCS})
