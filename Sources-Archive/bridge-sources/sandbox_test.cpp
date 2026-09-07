#include "Daad/SandboxValidator.hpp"
#include <iostream>
int main() {
  using daad::SandboxValidator;
  int fail = 0;
  auto check = [&](const char* name, const std::string& code, bool expectError) {
    SandboxValidator v;
    auto viols = v.validate(code);
    bool got = v.hasErrors();
    bool ok = (got == expectError);
    if (!ok) fail++;
    std::cout << (ok ? "PASS " : "FAIL ") << name
              << " (expectError=" << expectError << " gotError=" << got << ")\n";
  };
  check("SAFE-normal", "#include <iostream>\nint main(){return 0;}\n", false);
  check("UNSAFE-system", "int main(){system(\"rm -rf /\");}\n", true);
  check("UNSAFE-system-ws", "int main(){system   (\"x\");}\n", true);
  check("UNSAFE-popen", "int main(){popen(\"ls\",\"r\");}\n", true);
  check("UNSAFE-execl", "int main(){execl(\"/bin/sh\",0);}\n", true);
  check("UNSAFE-socket", "int main(){socket(1,2,3);}\n", true);
  check("UNSAFE-constcast", "int main(){const_cast<int&>(x);}\n", true);
  check("COMMENT-indented", "  // system(\"x\")\nint main(){return 0;}\n", false);
  check("DANGEROUS-header", "#include <unistd.h>\nint main(){return 0;}\n", true);
  return fail ? 1 : 0;
}
