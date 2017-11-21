.PHONY: clean All

All:
	@echo "----------Building project:[ StaticAnalysis - Debug ]----------"
	@cd "confResource" && "$(MAKE)" -f  "StaticAnalysis.mk"
clean:
	@echo "----------Cleaning project:[ StaticAnalysis - Debug ]----------"
	@cd "confResource" && "$(MAKE)" -f  "StaticAnalysis.mk" clean
