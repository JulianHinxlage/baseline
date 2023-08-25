//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Module.h"

int main(int argc, char* argv[]) {
	baseline::Module module;
	printf("module name: %s\n", module.name.c_str());
	return 0;
}
