.PHONY: clean All

All:
	@echo "----------Building project:[ ConfigInstrumentation - Debug ]----------"
	@cd "ConfigInstrumentation" && "$(MAKE)" -f  "ConfigInstrumentation.mk"
clean:
	@echo "----------Cleaning project:[ ConfigInstrumentation - Debug ]----------"
	@cd "ConfigInstrumentation" && "$(MAKE)" -f  "ConfigInstrumentation.mk" clean
