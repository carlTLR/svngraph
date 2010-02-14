#include "SVNRepository.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/graph/graphviz.hpp>

namespace scm {
  SVNRepository::SVNRepository()
  {
    apr_initialize();
    pool = svn_pool_create(NULL);
    targets = apr_array_make(pool, 1, sizeof(const char *));
  }

  SVNRepository& SVNRepository::init()
  {
    svn_error_t *err;
    /* Initialize and allocate the client_ctx object. */
    if ((err = svn_client_create_context (&ctx, pool))) {
      svn_handle_error2 (err, stderr, FALSE, "svngraph: ");
      // TODO: throw exception
    }

    /* Load the run-time config file into a hash */
    if((err = svn_config_get_config (&(ctx->config), NULL, pool))) {
      svn_handle_error2 (err, stderr, FALSE, "svngraph: ");
      // TODO: throw exception
    }

    svn_auth_provider_object_t *provider;
    apr_array_header_t *providers = apr_array_make (pool, 4, sizeof (svn_auth_provider_object_t *));

    svn_auth_get_username_provider(&provider, pool);
    APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

    /* Register the auth-providers into the context's auth_baton. */
    svn_auth_open (&ctx->auth_baton, providers, pool);
    return *this;
  }

  void SVNRepository::outLogGraph(std::string target)
  {
    APR_ARRAY_PUSH(targets, const char *) = target.c_str();

    apr_array_header_t *opt_ranges = apr_array_make(pool, 1, sizeof(svn_opt_revision_range_t *));
    svn_opt_revision_t revision;
    revision.kind = svn_opt_revision_head;
    svn_opt_revision_range_t *range = (svn_opt_revision_range_t *) apr_palloc(pool, sizeof(*range));
    range->start.kind = svn_opt_revision_number;
    range->start.value.number = 0;
    range->end.kind = svn_opt_revision_head;
    APR_ARRAY_PUSH(opt_ranges, svn_opt_revision_range_t *) = range;
    SVNRepository::log_receiver_baton baton;

    svn_error_t *err;
    if(err = svn_client_log5(targets, &revision, opt_ranges, 0, true, false, false, NULL, &SVNRepository::CreateLogGraph, &baton, ctx, pool)) {
      svn_handle_error2 (err, stderr, FALSE, "svngraph: ");
      // TODO: throw exception
    }

    // addMergeEdges(target, &baton);

    boost::write_graphviz(std::cout, baton.repoGraph, boost::make_label_writer(boost::get(&SVNRepository::SvnRevision::revNum, baton.repoGraph)));
  }

  void SVNRepository::addMergeEdges(std::string target, SVNRepository::log_receiver_baton *lb)
  {
    apr_hash_t **props;
    svn_error_t *err;
    svn_opt_revision_t revision, peg_revision;
    revision.kind = svn_opt_revision_unspecified;
    peg_revision.kind = svn_opt_revision_unspecified;
    const char *path = target.c_str();

    if(err = svn_client_propget3(props, "svn:mergeinfo", path, &peg_revision, &revision, NULL, svn_depth_immediates, NULL, ctx, pool)) {
      svn_handle_error2 (err, stderr, FALSE, "svngraph: ");
      // TODO: throw exception
    } else {
      apr_hash_index_t *hi;
      void *val;
      const void *key;
      for (hi = apr_hash_first(pool, *props); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, &key, NULL, &val);
        const std::string key_s = (const char *) key;
        const std::string val_s = ((svn_string_t *) val)->data;

        std::cout << key_s << "   " << val_s << std::endl;
      }
    }
  }

  svn_error_t * SVNRepository::CreateLogGraph(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
  {
    SVNRepository::log_receiver_baton *lb = (SVNRepository::log_receiver_baton *) baton;
    std::vector<boost::regex> branch_patterns;
    branch_patterns.push_back(boost::regex("[\\/]?[^\\/]*\\/trunk"));
    branch_patterns.push_back(boost::regex("[\\/]?[^\\/]*\\/branches\\/[^\\/]+"));
    branch_patterns.push_back(boost::regex("[\\/]?[^\\/]*\\/tags\\/[^\\/]+"));
    boost::smatch matches;

    apr_hash_index_t *hi;
    void *val;
    const void *key;

    if(log_entry->revision > 0) {
      for (hi = apr_hash_first(pool, log_entry->changed_paths2); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, &key, NULL, &val);
        const std::string key_s = (const char *) key;
        svn_log_changed_path2_t * val_s = (svn_log_changed_path2_t *) val;

        for(std::vector<boost::regex>::const_iterator branch_patternsI = branch_patterns.begin();
            branch_patternsI != branch_patterns.end(); branch_patternsI++) {
          if(boost::regex_search(key_s.begin(), key_s.end(), matches, *branch_patternsI)) {
            std::string branch = std::string(matches[0].first, matches[0].second);
            if(lb->branch_head[branch].first == 0 || lb->branch_head[branch].first != log_entry->revision) {
              SVNRepository::AdjListTraits::vertex_descriptor vertex_id_old, vertex_id_new;
              vertex_id_new = boost::add_vertex(SVNRepository::SvnRevision((int) log_entry->revision, branch), lb->repoGraph);

              if(lb->branch_head[branch].first > 0) {
                boost::add_edge(lb->branch_head[branch].second, vertex_id_new, lb->repoGraph);
              }

              lb->branch_head[branch].first = log_entry->revision;
              lb->branch_head[branch].second = vertex_id_new;

              if(val_s->copyfrom_rev > 0 && val_s->copyfrom_path != NULL) {
                boost::property_map<SVNRepository::RepoGraph, std::pair<int, std::string> SVNRepository::SvnRevision::*>::type pMap = boost::get(&SVNRepository::SvnRevision::branch_rev, lb->repoGraph);
                boost::graph_traits<SVNRepository::RepoGraph>::vertex_iterator vi, vi_end, next;
                tie(vi, vi_end) = vertices(lb->repoGraph);
                for (next = vi; vi != vi_end; vi = next) {
                  ++next;
                  if(boost::get(pMap, *vi).first == val_s->copyfrom_rev && boost::get(pMap, *vi).second == val_s->copyfrom_path) {
                    boost::add_edge(*vi, vertex_id_new, lb->repoGraph);
                  }
                }
              }
            }
            break;
          }
        }
      }
    }

    return NULL;
  }
}
