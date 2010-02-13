#include "RepositoryFactory.h"
#include "SVNRepository.h"

namespace scm {
  Repository* RepositoryFactory::GetRepository(scmType t) {
    switch(t) {
      case SVN:
        return new SVNRepository();
        break;
    }
  }
}
