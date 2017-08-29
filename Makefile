.PHONY: clean All

All:
	@echo "----------Building project:[ confResource - Debug ]----------"
	@cd "confResource" && "$(MAKE)" -f  "confResource.mk"
clean:
	@echo "----------Cleaning project:[ confResource - Debug ]----------"
	@cd "confResource" && "$(MAKE)" -f  "confResource.mk" clean
