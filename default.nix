with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "tsm";
  src = ./.;
  enableParallelBuilding = true;

  cmakeFlags = ["-GNinja -DGTEST_INCLUDE_DIR=${gtest}/include -DBUILD_COVERAGE=ON"];
  
  nativeBuildInputs = [cmake ninja];

  buildInputs = [gflags glog gtest graphviz doxygen] ++ (if stdenv.isDarwin then [llvm] else if stdenv.isLinux then [lcov gcc] else throw "unsupported platform");

  buildPhase = ''
    cmake --build . --  tsm_all && cmake --build . -- install
  '';

  meta = with stdenv.lib; {
    description = "tsm, a c++ state machine framework";
    platforms = with platforms; darwin ++ linux;
    license = licenses.mit;
  };
}