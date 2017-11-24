.PHONY: clean All

All:
	@echo "----------Building project:[ DynamicAnalysis - Debug ]----------"
	@cd "Dynamic" && "$(MAKE)" -f  "DynamicAnalysis.mk"
clean:
	@echo "----------Cleaning project:[ DynamicAnalysis - Debug ]----------"
	@cd "Dynamic" && "$(MAKE)" -f  "DynamicAnalysis.mk" clean
