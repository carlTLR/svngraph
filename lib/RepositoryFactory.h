#ifndef _REPOSITORYFACTORY_H
#define _REPOSITORYFACTORY_H 1

#include "Repository.h"

namespace scm {
  enum scmType {SVN};

  class RepositoryFactory
  {
    // Public
    public:
      static Repository* GetRepository(scmType);
  };
}
#endif /* !_REPOSITORYFACTORY_H */
