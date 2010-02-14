#ifndef _REPOSITORY_H
#define _REPOSITORY_H 1

#include <string>

namespace scm {
  class Repository
  {
    public:
      // Object methods
      virtual void outLogGraph(std::string) = 0;
      virtual Repository& init() = 0;
  };
}

#endif /* !_REPOSITORY_H */
