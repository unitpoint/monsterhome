	.arch armv4t
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"AdmobAds_register.cpp"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.section	.rodata
	.align	2
.LC0:
	.ascii	"ADMOBADS_VERBOSE\000"
	.align	2
.LC1:
	.ascii	"calling AdmobAds func on main thread: InitAds\000"
	.section	.text._ZL12InitAds_wrapPKc,"ax",%progbits
	.align	2
	.type	_ZL12InitAds_wrapPKc, %function
_ZL12InitAds_wrapPKc:
.LFB15:
	.file 1 "C:\\marmalade\\marmalade-projects\\monsterhome\\extensions\\admobads\\source\\generic\\AdmobAds_register.cpp"
	.loc 1 28 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
.LCFI0:
	sub	sp, sp, #8
.LCFI1:
	.loc 1 30 0
	ldr	r4, .L4
.LPIC3:
	add	r4, pc, r4
	.loc 1 28 0
	str	r0, [sp, #4]
	.loc 1 29 0
	ldr	r3, .L4+4
.LPIC0:
	add	r3, pc, r3
	mov	r0, r3
	mov	r1, #1
	bl	IwDebugTraceIsChannelOn(PLT)
	mov	r3, r0
	cmp	r3, #0
	moveq	r3, #0
	movne	r3, #1
	and	r3, r3, #255
	cmp	r3, #0
	beq	.L2
	ldr	r3, .L4+8
.LPIC1:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceSetTraceChannel(PLT)
	ldr	r3, .L4+12
.LPIC2:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceLinePrintf(PLT)
.L2:
	.loc 1 30 0
	ldr	r3, .L4+16
	ldr	r3, [r4, r3]
	mov	r0, r3
	mov	r1, #1
	ldr	r2, [sp, #4]
	bl	s3eEdkThreadRunOnOS(PLT)
	mov	r3, r0
	.loc 1 31 0
	mov	r0, r3
	add	sp, sp, #8
	ldmfd	sp!, {r4, lr}
	bx	lr
.L5:
	.align	2
.L4:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC3+8)
	.word	.LC0-(.LPIC0+8)
	.word	.LC0-(.LPIC1+8)
	.word	.LC1-(.LPIC2+8)
	.word	InitAds(GOT)
.LFE15:
	.size	_ZL12InitAds_wrapPKc, .-_ZL12InitAds_wrapPKc
	.section	.rodata
	.align	2
.LC2:
	.ascii	"calling AdmobAds func on main thread: ShowAds\000"
	.section	.text._ZL12ShowAds_wrapv,"ax",%progbits
	.align	2
	.type	_ZL12ShowAds_wrapv, %function
_ZL12ShowAds_wrapv:
.LFB16:
	.loc 1 34 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
.LCFI2:
	.loc 1 36 0
	ldr	r4, .L9
.LPIC7:
	add	r4, pc, r4
	.loc 1 35 0
	ldr	r3, .L9+4
.LPIC4:
	add	r3, pc, r3
	mov	r0, r3
	mov	r1, #1
	bl	IwDebugTraceIsChannelOn(PLT)
	mov	r3, r0
	cmp	r3, #0
	moveq	r3, #0
	movne	r3, #1
	and	r3, r3, #255
	cmp	r3, #0
	beq	.L7
	ldr	r3, .L9+8
.LPIC5:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceSetTraceChannel(PLT)
	ldr	r3, .L9+12
.LPIC6:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceLinePrintf(PLT)
.L7:
	.loc 1 36 0
	ldr	r3, .L9+16
	ldr	r3, [r4, r3]
	mov	r0, r3
	mov	r1, #0
	bl	s3eEdkThreadRunOnOS(PLT)
	mov	r3, r0
	.loc 1 37 0
	mov	r0, r3
	ldmfd	sp!, {r4, lr}
	bx	lr
.L10:
	.align	2
.L9:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC7+8)
	.word	.LC0-(.LPIC4+8)
	.word	.LC0-(.LPIC5+8)
	.word	.LC2-(.LPIC6+8)
	.word	ShowAds(GOT)
.LFE16:
	.size	_ZL12ShowAds_wrapv, .-_ZL12ShowAds_wrapv
	.section	.rodata
	.align	2
.LC3:
	.ascii	"calling AdmobAds func on main thread: HideAds\000"
	.section	.text._ZL12HideAds_wrapv,"ax",%progbits
	.align	2
	.type	_ZL12HideAds_wrapv, %function
_ZL12HideAds_wrapv:
.LFB17:
	.loc 1 40 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
.LCFI3:
	.loc 1 42 0
	ldr	r4, .L14
.LPIC11:
	add	r4, pc, r4
	.loc 1 41 0
	ldr	r3, .L14+4
.LPIC8:
	add	r3, pc, r3
	mov	r0, r3
	mov	r1, #1
	bl	IwDebugTraceIsChannelOn(PLT)
	mov	r3, r0
	cmp	r3, #0
	moveq	r3, #0
	movne	r3, #1
	and	r3, r3, #255
	cmp	r3, #0
	beq	.L12
	ldr	r3, .L14+8
.LPIC9:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceSetTraceChannel(PLT)
	ldr	r3, .L14+12
.LPIC10:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceLinePrintf(PLT)
.L12:
	.loc 1 42 0
	ldr	r3, .L14+16
	ldr	r3, [r4, r3]
	mov	r0, r3
	mov	r1, #0
	bl	s3eEdkThreadRunOnOS(PLT)
	mov	r3, r0
	.loc 1 43 0
	mov	r0, r3
	ldmfd	sp!, {r4, lr}
	bx	lr
.L15:
	.align	2
.L14:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC11+8)
	.word	.LC0-(.LPIC8+8)
	.word	.LC0-(.LPIC9+8)
	.word	.LC3-(.LPIC10+8)
	.word	HideAds(GOT)
.LFE17:
	.size	_ZL12HideAds_wrapv, .-_ZL12HideAds_wrapv
	.section	.rodata
	.align	2
.LC4:
	.ascii	"calling AdmobAds func on main thread: hasAdLoaded\000"
	.section	.text._ZL16hasAdLoaded_wrapv,"ax",%progbits
	.align	2
	.type	_ZL16hasAdLoaded_wrapv, %function
_ZL16hasAdLoaded_wrapv:
.LFB18:
	.loc 1 46 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
.LCFI4:
	.loc 1 48 0
	ldr	r4, .L19
.LPIC15:
	add	r4, pc, r4
	.loc 1 47 0
	ldr	r3, .L19+4
.LPIC12:
	add	r3, pc, r3
	mov	r0, r3
	mov	r1, #1
	bl	IwDebugTraceIsChannelOn(PLT)
	mov	r3, r0
	cmp	r3, #0
	moveq	r3, #0
	movne	r3, #1
	and	r3, r3, #255
	cmp	r3, #0
	beq	.L17
	ldr	r3, .L19+8
.LPIC13:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceSetTraceChannel(PLT)
	ldr	r3, .L19+12
.LPIC14:
	add	r3, pc, r3
	mov	r0, r3
	bl	IwDebugTraceLinePrintf(PLT)
.L17:
	.loc 1 48 0
	ldr	r3, .L19+16
	ldr	r3, [r4, r3]
	mov	r0, r3
	mov	r1, #0
	bl	s3eEdkThreadRunOnOS(PLT)
	mov	r3, r0
	cmp	r3, #0
	moveq	r3, #0
	movne	r3, #1
	and	r3, r3, #255
	.loc 1 49 0
	mov	r0, r3
	ldmfd	sp!, {r4, lr}
	bx	lr
.L20:
	.align	2
.L19:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC15+8)
	.word	.LC0-(.LPIC12+8)
	.word	.LC0-(.LPIC13+8)
	.word	.LC4-(.LPIC14+8)
	.word	hasAdLoaded(GOT)
.LFE18:
	.size	_ZL16hasAdLoaded_wrapv, .-_ZL16hasAdLoaded_wrapv
	.section	.rodata
	.align	2
.LC5:
	.ascii	"AdmobAds\000"
	.section	.text._Z19AdmobAdsRegisterExtv,"ax",%progbits
	.align	2
	.global	_Z19AdmobAdsRegisterExtv
	.hidden	_Z19AdmobAdsRegisterExtv
	.type	_Z19AdmobAdsRegisterExtv, %function
_Z19AdmobAdsRegisterExtv:
.LFB19:
	.loc 1 59 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
.LCFI5:
	sub	sp, sp, #52
.LCFI6:
.LBB2:
	.loc 1 75 0
	ldr	r2, .L23
.LPIC20:
	add	r2, pc, r2
	.loc 1 62 0
	ldr	r3, .L23+4
.LPIC16:
	add	r3, pc, r3
	str	r3, [sp, #32]
	.loc 1 63 0
	ldr	r3, .L23+8
.LPIC17:
	add	r3, pc, r3
	str	r3, [sp, #36]
	.loc 1 64 0
	ldr	r3, .L23+12
.LPIC18:
	add	r3, pc, r3
	str	r3, [sp, #40]
	.loc 1 65 0
	ldr	r3, .L23+16
.LPIC19:
	add	r3, pc, r3
	str	r3, [sp, #44]
	.loc 1 70 0
	add	r3, sp, #16
	mov	r1, #0
	str	r1, [r3, #0]
	add	r3, r3, #4
	mov	r1, #0
	str	r1, [r3, #0]
	add	r3, r3, #4
	mov	r1, #0
	str	r1, [r3, #0]
	add	r3, r3, #4
	mov	r1, #0
	str	r1, [r3, #0]
	add	r3, r3, #4
	.loc 1 75 0
	add	r1, sp, #32
	add	ip, sp, #16
	ldr	r3, .L23+20
	ldr	r3, [r2, r3]
	str	r3, [sp, #0]
	ldr	r3, .L23+24
	ldr	r3, [r2, r3]
	str	r3, [sp, #4]
	mov	r3, #0
	str	r3, [sp, #8]
	ldr	r3, .L23+28
.LPIC21:
	add	r3, pc, r3
	mov	r0, r3
	mov	r2, #16
	mov	r3, ip
	bl	s3eEdkRegister(PLT)
.LBE2:
	.loc 1 76 0
	add	sp, sp, #52
	ldr	lr, [sp], #4
	bx	lr
.L24:
	.align	2
.L23:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC20+8)
	.word	_ZL12InitAds_wrapPKc-(.LPIC16+8)
	.word	_ZL12ShowAds_wrapv-(.LPIC17+8)
	.word	_ZL12HideAds_wrapv-(.LPIC18+8)
	.word	_ZL16hasAdLoaded_wrapv-(.LPIC19+8)
	.word	_Z12AdmobAdsInitv(GOT)
	.word	_Z17AdmobAdsTerminatev(GOT)
	.word	.LC5-(.LPIC21+8)
.LFE19:
	.size	_Z19AdmobAdsRegisterExtv, .-_Z19AdmobAdsRegisterExtv
	.section	.text.RegisterExt,"ax",%progbits
	.align	2
	.global	RegisterExt
	.type	RegisterExt, %function
RegisterExt:
.LFB20:
	.loc 1 95 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
.LCFI7:
	.loc 1 96 0
	bl	_Z19AdmobAdsRegisterExtv(PLT)
	.loc 1 97 0
	ldmfd	sp!, {r4, lr}
	bx	lr
.LFE20:
	.size	RegisterExt, .-RegisterExt
	.section	.debug_frame,"",%progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.byte	0xe
	.byte	0xc
	.uleb128 0xd
	.uleb128 0x0
	.align	2
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB15
	.4byte	.LFE15-.LFB15
	.byte	0x4
	.4byte	.LCFI0-.LFB15
	.byte	0xe
	.uleb128 0x8
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0xe
	.uleb128 0x10
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.byte	0x11
	.uleb128 0x4
	.sleb128 2
	.align	2
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB16
	.4byte	.LFE16-.LFB16
	.byte	0x4
	.4byte	.LCFI2-.LFB16
	.byte	0xe
	.uleb128 0x8
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.byte	0x11
	.uleb128 0x4
	.sleb128 2
	.align	2
.LEFDE2:
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB17
	.4byte	.LFE17-.LFB17
	.byte	0x4
	.4byte	.LCFI3-.LFB17
	.byte	0xe
	.uleb128 0x8
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.byte	0x11
	.uleb128 0x4
	.sleb128 2
	.align	2
.LEFDE4:
.LSFDE6:
	.4byte	.LEFDE6-.LASFDE6
.LASFDE6:
	.4byte	.Lframe0
	.4byte	.LFB18
	.4byte	.LFE18-.LFB18
	.byte	0x4
	.4byte	.LCFI4-.LFB18
	.byte	0xe
	.uleb128 0x8
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.byte	0x11
	.uleb128 0x4
	.sleb128 2
	.align	2
.LEFDE6:
.LSFDE8:
	.4byte	.LEFDE8-.LASFDE8
.LASFDE8:
	.4byte	.Lframe0
	.4byte	.LFB19
	.4byte	.LFE19-.LFB19
	.byte	0x4
	.4byte	.LCFI5-.LFB19
	.byte	0xe
	.uleb128 0x4
	.byte	0x4
	.4byte	.LCFI6-.LCFI5
	.byte	0xe
	.uleb128 0x38
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.align	2
.LEFDE8:
.LSFDE10:
	.4byte	.LEFDE10-.LASFDE10
.LASFDE10:
	.4byte	.Lframe0
	.4byte	.LFB20
	.4byte	.LFE20-.LFB20
	.byte	0x4
	.4byte	.LCFI7-.LFB20
	.byte	0xe
	.uleb128 0x8
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.byte	0x11
	.uleb128 0x4
	.sleb128 2
	.align	2
.LEFDE10:
	.text
.Letext0:
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB15
	.4byte	.LCFI0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI0
	.4byte	.LCFI1
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	.LCFI1
	.4byte	.LFE15
	.2byte	0x2
	.byte	0x7d
	.sleb128 16
	.4byte	0x0
	.4byte	0x0
.LLST1:
	.4byte	.LFB16
	.4byte	.LCFI2
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI2
	.4byte	.LFE16
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
.LLST2:
	.4byte	.LFB17
	.4byte	.LCFI3
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI3
	.4byte	.LFE17
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
.LLST3:
	.4byte	.LFB18
	.4byte	.LCFI4
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI4
	.4byte	.LFE18
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
.LLST4:
	.4byte	.LFB19
	.4byte	.LCFI5
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI5
	.4byte	.LCFI6
	.2byte	0x2
	.byte	0x7d
	.sleb128 4
	.4byte	.LCFI6
	.4byte	.LFE19
	.2byte	0x2
	.byte	0x7d
	.sleb128 56
	.4byte	0x0
	.4byte	0x0
.LLST5:
	.4byte	.LFB20
	.4byte	.LCFI7
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI7
	.4byte	.LFE20
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
	.file 2 "../../../../../6.0/s3e/h/s3eTypes.h"
	.section	.debug_info
	.4byte	0x199
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF18
	.byte	0x4
	.4byte	.LASF19
	.4byte	.LASF20
	.4byte	0x0
	.4byte	0x0
	.4byte	.Ldebug_ranges0+0x0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF3
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x4
	.4byte	.LASF21
	.byte	0x4
	.byte	0x2
	.byte	0xe2
	.4byte	0x81
	.uleb128 0x5
	.4byte	.LASF8
	.sleb128 0
	.uleb128 0x5
	.4byte	.LASF9
	.sleb128 1
	.byte	0x0
	.uleb128 0x6
	.4byte	.LASF21
	.byte	0x2
	.byte	0xe6
	.4byte	0x68
	.uleb128 0x7
	.byte	0x4
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF10
	.uleb128 0x8
	.byte	0x4
	.byte	0x7
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF11
	.uleb128 0x9
	.byte	0x4
	.4byte	0xa5
	.uleb128 0xa
	.4byte	0x98
	.uleb128 0xb
	.4byte	0x8c
	.uleb128 0xc
	.4byte	.LASF22
	.byte	0x1
	.byte	0x1b
	.4byte	0x81
	.4byte	.LFB15
	.4byte	.LFE15
	.4byte	.LLST0
	.4byte	0xd9
	.uleb128 0xd
	.4byte	.LASF23
	.byte	0x1
	.byte	0x1b
	.4byte	0x9f
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.byte	0x0
	.uleb128 0xe
	.4byte	.LASF12
	.byte	0x1
	.byte	0x21
	.4byte	0x81
	.4byte	.LFB16
	.4byte	.LFE16
	.4byte	.LLST1
	.uleb128 0xe
	.4byte	.LASF13
	.byte	0x1
	.byte	0x27
	.4byte	0x81
	.4byte	.LFB17
	.4byte	.LFE17
	.4byte	.LLST2
	.uleb128 0xe
	.4byte	.LASF14
	.byte	0x1
	.byte	0x2d
	.4byte	0x11e
	.4byte	.LFB18
	.4byte	.LFE18
	.4byte	.LLST3
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.4byte	.LASF15
	.uleb128 0xf
	.byte	0x1
	.4byte	.LASF24
	.byte	0x1
	.byte	0x3a
	.4byte	.LASF25
	.4byte	.LFB19
	.4byte	.LFE19
	.4byte	.LLST4
	.4byte	0x168
	.uleb128 0x10
	.4byte	.LBB2
	.4byte	.LBE2
	.uleb128 0x11
	.4byte	.LASF16
	.byte	0x1
	.byte	0x3d
	.4byte	0x168
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x11
	.4byte	.LASF17
	.byte	0x1
	.byte	0x46
	.4byte	0x178
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.4byte	0x8c
	.4byte	0x178
	.uleb128 0x13
	.4byte	0x95
	.byte	0x3
	.byte	0x0
	.uleb128 0x12
	.4byte	0x4c
	.4byte	0x188
	.uleb128 0x13
	.4byte	0x95
	.byte	0x3
	.byte	0x0
	.uleb128 0x14
	.byte	0x1
	.4byte	.LASF26
	.byte	0x1
	.byte	0x5e
	.4byte	.LFB20
	.4byte	.LFE20
	.4byte	.LLST5
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x4
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x28
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x15
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x10
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x11
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x36
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x19d
	.4byte	0x125
	.ascii	"AdmobAdsRegisterExt\000"
	.4byte	0x188
	.ascii	"RegisterExt\000"
	.4byte	0x0
	.section	.debug_aranges,"",%progbits
	.4byte	0x44
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.LFB15
	.4byte	.LFE15-.LFB15
	.4byte	.LFB16
	.4byte	.LFE16-.LFB16
	.4byte	.LFB17
	.4byte	.LFE17-.LFB17
	.4byte	.LFB18
	.4byte	.LFE18-.LFB18
	.4byte	.LFB19
	.4byte	.LFE19-.LFB19
	.4byte	.LFB20
	.4byte	.LFE20-.LFB20
	.4byte	0x0
	.4byte	0x0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LFB15
	.4byte	.LFE15
	.4byte	.LFB16
	.4byte	.LFE16
	.4byte	.LFB17
	.4byte	.LFE17
	.4byte	.LFB18
	.4byte	.LFE18
	.4byte	.LFB19
	.4byte	.LFE19
	.4byte	.LFB20
	.4byte	.LFE20
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",%progbits,1
.LASF5:
	.ascii	"long long int\000"
.LASF2:
	.ascii	"short unsigned int\000"
.LASF4:
	.ascii	"unsigned int\000"
.LASF0:
	.ascii	"unsigned char\000"
.LASF17:
	.ascii	"flags\000"
.LASF8:
	.ascii	"S3E_RESULT_SUCCESS\000"
.LASF9:
	.ascii	"S3E_RESULT_ERROR\000"
.LASF19:
	.ascii	"C:\\marmalade\\marmalade-projects\\monsterhome\\ext"
	.ascii	"ensions\\admobads\\source\\generic\\AdmobAds_regist"
	.ascii	"er.cpp\000"
.LASF7:
	.ascii	"long unsigned int\000"
.LASF6:
	.ascii	"long long unsigned int\000"
.LASF13:
	.ascii	"HideAds_wrap\000"
.LASF18:
	.ascii	"GNU C++ 4.4.3\000"
.LASF22:
	.ascii	"InitAds_wrap\000"
.LASF21:
	.ascii	"s3eResult\000"
.LASF11:
	.ascii	"char\000"
.LASF15:
	.ascii	"bool\000"
.LASF3:
	.ascii	"short int\000"
.LASF24:
	.ascii	"AdmobAdsRegisterExt\000"
.LASF25:
	.ascii	"_Z19AdmobAdsRegisterExtv\000"
.LASF26:
	.ascii	"RegisterExt\000"
.LASF23:
	.ascii	"pub_id\000"
.LASF1:
	.ascii	"signed char\000"
.LASF10:
	.ascii	"long int\000"
.LASF20:
	.ascii	"c:\\\\marmalade\\\\marmalade-projects\\\\monsterhom"
	.ascii	"e\\\\extensions\\\\admobads\\\\build_admobads_andro"
	.ascii	"id_linux_scons_arm\000"
.LASF14:
	.ascii	"hasAdLoaded_wrap\000"
.LASF16:
	.ascii	"funcPtrs\000"
.LASF12:
	.ascii	"ShowAds_wrap\000"
	.ident	"GCC: (GNU) 4.4.3"
	.section	.note.GNU-stack,"",%progbits
