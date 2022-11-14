.PHONY: clean All

All:
	@echo "----------Building project:[ basicd - Debug ]----------"
	@"$(MAKE)" -f  "basicd.mk"
clean:
	@echo "----------Cleaning project:[ basicd - Debug ]----------"
	@"$(MAKE)" -f  "basicd.mk" clean
