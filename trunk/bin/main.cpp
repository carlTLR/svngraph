#include "Repository.h"
#include "RepositoryFactory.h"
#include <iostream>

int main(int argc, char *argv[]) {
  scm::Repository *repo = scm::RepositoryFactory::GetRepository(scm::SVN);

  if (argc <= 1) {
    std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
    return 1;
  } else {
    repo->init();
    repo->getLogGraph(argv[1]);
  }
}
