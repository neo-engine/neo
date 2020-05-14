OUT			:=	../FSROOT/SOUND
HEADER_OUT	:=	../arm9/build
MUSIC		:=	.

WAVFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.wav)),$(MUSIC)/$(dir))
MODFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.mod)),$(MUSIC)/$(dir))
MODFILES	+=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.s3m)),$(MUSIC)/$(dir))
MODFILES	+=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.it)),$(MUSIC)/$(dir))
MODFILES	+=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.xm)),$(MUSIC)/$(dir))

#TARGETS		:= $(MODFILES:.mod=.msl)
#TARGETS		+= $(MODFILES:.s3m=.msl)
#TARGETS		+= $(MODFILES:.it=.msl)
#TARGETS		+= $(MODFILES:.xm=.msl)

all	:	sound.msl
	@echo crunching sound

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
sound.msl	:	$(WAVFILES) $(MODFILES)
#---------------------------------------------------------------------------------
	@mkdir -p $(OUT)/$(@D)
	@mmutil $^ -d -o$(OUT)/sound.msl -h$(HEADER_OUT)/soundbank.h

clean	:
	@echo cleaning sound
	@rm -rf $(OUT)/sound.msl
