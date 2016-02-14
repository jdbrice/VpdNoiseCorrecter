

#include "XmlConfig.h"
#include "Engine.h"
using namespace jdb;

#include <iostream>
#include <exception>

#include "VpdNoiseCorrecter.h"

int main( int argc, char* argv[] ) {

	TaskFactory::registerTaskRunner<VpdNoiseCorrecter>( "VpdNoiseCorrecter" );
	Engine engine( argc, argv );

	return 0;
}
