with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "tsm";
  src = ./.;
  enableParallelBuilding = true;

  cmakeFlags = ["-GNinja -DGTEST_INCLUDE_DIR=${gtest}/include -DBUILD_COVERAGE=ON"];
  
  buildInputs = [cmake ninja gflags glog gtest llvm graphviz doxygen];

  buildPhase = ''
    cmake --build . --  tsm_all && cmake --build . -- install
  '';

  meta = with stdenv.lib; {
    description = "tsm, a c++ state machine framework";
    platforms = with platforms; darwin ++ linux;
    license = licenses.mit;
  };
}