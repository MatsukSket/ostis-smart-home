#include "TestUtils.hpp"

void LoadTestStructure(ScAgentContext & context, std::string const & filename)
{
  ScsLoader loader;
  loader.loadScsFile(context, TEST_FILES_DIR + filename);
}
