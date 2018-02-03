.PHONY: clean All

All:
	@echo "----------Building project:[ PreCompilerHandler - Debug ]----------"
	@cd "PreCompilerHandler" && "$(MAKE)" -f  "PreCompilerHandler.mk"
clean:
	@echo "----------Cleaning project:[ PreCompilerHandler - Debug ]----------"
	@cd "PreCompilerHandler" && "$(MAKE)" -f  "PreCompilerHandler.mk" clean
