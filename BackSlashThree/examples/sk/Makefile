.PHONY = all clean sk.bs3

BS3ASM = @../../bs3tinyc.sh

all: intro clean gen_sk_asset sk.bs3

intro:
	@echo "Get Silly Knight resources at http://www.doshaven.eu/game/silly-knight"
	@echo " copy Sily Knight DATA folder here to have ./DATA folder available "

gen_sk_asset:
	@echo ">>> Creating/Compiling sk assets"
	@./sk-asm.sh ./DATA .

clean:
	@echo ">>> Cleaning files"
	@rm -fv *.s
	@rm -fv *.bs3
	@rm -fv *.rpt
	@rm -fv sk

sk.bs3: sk.tc 
	@echo ">>> Create sk.* from sk.tc"
	$(BS3ASM) sk.tc

