#ifndef _SVNREPOSITORY_H
#define _SVNREPOSITORY_H 1

#include "Repository.h"
#include <svn_pools.h>
#include <svn_client.h>
#include <svn_cmdline.h>
#include <svn_compat.h>
#include <svn_opt.h>
#include <map>
#include <boost/graph/adjacency_list.hpp>

namespace scm {
  struct SvnRevision {
    int revNum;
    std::string branch;
    std::pair<int, std::string> branch_rev;
    SvnRevision(int r_num=0, std::string b="") : revNum(r_num), branch(b), branch_rev(revNum, branch) {};
  };

  typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> AdjListTraits;
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, SvnRevision> RepoGraph;

  struct log_receiver_baton {
    std::map<std::string, std::pair<int, AdjListTraits::vertex_descriptor> > branch_head;
    RepoGraph repoGraph;
  };

  class SVNRepository : public Repository
  {
    public:
      // Constructors
      SVNRepository();

      //Destructor
      ~SVNRepository() { svn_pool_destroy(pool); delete[] targets; delete ctx; };

      // Static methods
      static svn_error_t* CreateLogGraph(void *, svn_log_entry_t *, apr_pool_t *);

      // Object methods
      std::string getLogGraph(std::string);
      SVNRepository& init();

    private:
      // Members
      apr_pool_t *pool;
      apr_array_header_t *targets;
      svn_client_ctx_t *ctx;
  };
}

#endif /* !_SVNREPOSITORY_H */
