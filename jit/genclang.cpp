# include <stdlib.h>
# include <stdint.h>
# ifndef WIN32
# include <unistd.h>
# endif
# include <new>
# include <math.h>
# include <stdio.h>
extern "C" {
# include "lpc_ext.h"
}
# include "data.h"
# include "instruction.h"
# include "code.h"
# include "stack.h"
# include "block.h"
# include "typed.h"
# include "flow.h"
# ifndef WIN32
# include "gentt.h"
# else
# define TARGET_TRIPLE "i686-pc-windows-msvc"
# endif
# include "genclang.h"
# include "jitcomp.h"

# define Int		"i32"
# define Double		"double"
# define INT_SIZE	4
# define DOUBLE_SIZE	8
# undef  LLVM3_6	/* generate IR for LLVM 3.5 and 3.6 */

static const struct {
    const char *ret;			/* return value */
    const char *args;			/* function arguments */
} functions[] = {
# define VM_INT				0
    { "void", "(i8*, " Int ")" },
# define VM_FLOAT			1
    { "void", "(i8*, " Double ")" },
# define VM_STRING			2
    { "void", "(i8*, i16, i16)" },
# define VM_PARAM			3
    { "void", "(i8*, i8)" },
# define VM_PARAM_INT			4
    { Int, "(i8*, i8)" },
# define VM_PARAM_FLOAT			5
    { Double, "(i8*, i8)" },
# define VM_LOCAL			6
    { "void", "(i8*, i8)" },
# define VM_LOCAL_INT			7
    { Int, "(i8*, i8)" },
# define VM_LOCAL_FLOAT			8
    { Double, "(i8*, i8)" },
# define VM_GLOBAL			9
    { "void", "(i8*, i16, i8)" },
# define VM_GLOBAL_INT			10
    { Int, "(i8*, i16, i8)" },
# define VM_GLOBAL_FLOAT		11
    { Double, "(i8*, i16, i8)" },
# define VM_INDEX			12
    { "void", "(i8*)" },
# define VM_INDEX_INT			13
    { Int, "(i8*)" },
# define VM_INDEX2			14
    { "void", "(i8*)" },
# define VM_INDEX2_INT			15
    { Int, "(i8*)" },
# define VM_AGGREGATE			16
    { "void", "(i8*, i16)" },
# define VM_MAP_AGGREGATE		17
    { "void", "(i8*, i16)" },
# define VM_CAST			18
    { "void", "(i8*, i8, i16, i16)" },
# define VM_CAST_INT			19
    { Int, "(i8*)" },
# define VM_CAST_FLOAT			20
    { Double, "(i8*)" },
# define VM_INSTANCEOF			21
    { Int, "(i8*, i16, i16)" },
# define VM_RANGE			22
    { "void", "(i8*)" },
# define VM_RANGE_FROM			23
    { "void", "(i8*)" },
# define VM_RANGE_TO			24
    { "void", "(i8*)" },
# define VM_STORE_PARAM			25
    { "void", "(i8*, i8)" },
# define VM_STORE_PARAM_INT		26
    { "void", "(i8*, i8, " Int ")" },
# define VM_STORE_PARAM_FLOAT		27
    { "void", "(i8*, i8, " Double ")" },
# define VM_STORE_LOCAL			28
    { "void", "(i8*, i8)" },
# define VM_STORE_LOCAL_INT		29
    { "void", "(i8*, i8, " Int ")" },
# define VM_STORE_LOCAL_FLOAT		30
    { "void", "(i8*, i8, " Double ")" },
# define VM_STORE_GLOBAL		31
    { "void", "(i8*, i16, i8)" },
# define VM_STORE_GLOBAL_INT		32
    { "void", "(i8*, i16, i8, " Int ")" },
# define VM_STORE_GLOBAL_FLOAT		33
    { "void", "(i8*, i16, i8, " Double ")" },
# define VM_STORE_INDEX			34
    { "void", "(i8*)" },
# define VM_STORE_PARAM_INDEX		35
    { "void", "(i8*, i8)" },
# define VM_STORE_LOCAL_INDEX		36
    { "void", "(i8*, i8)" },
# define VM_STORE_GLOBAL_INDEX		37
    { "void", "(i8*, i16, i8)" },
# define VM_STORE_INDEX_INDEX		38
    { "void", "(i8*)" },
# define VM_STORES			39
    { "void", "(i8*, i8)" },
# define VM_STORES_LVAL			40
    { "void", "(i8*, i8)" },
# define VM_STORES_SPREAD		41
    { "void", "(i8*, i8, i8, i8, i16, i16)" },
# define VM_STORES_CAST			42
    { "void", "(i8*, i8, i16, i16)" },
# define VM_STORES_PARAM		43
    { "void", "(i8*, i8)" },
# define VM_STORES_PARAM_INT		44
    { Int, "(i8*, i8)" },
# define VM_STORES_PARAM_FLOAT		45
    { Double, "(i8*, i8)" },
# define VM_STORES_LOCAL		46
    { "void", "(i8*, i8)" },
# define VM_STORES_LOCAL_INT		47
    { Int, "(i8*, i8, " Int ")" },
# define VM_STORES_LOCAL_FLOAT		48
    { Double, "(i8*, i8, " Double ")" },
# define VM_STORES_GLOBAL		49
    { "void", "(i8*, i16, i8)" },
# define VM_STORES_INDEX		50
    { "void", "(i8*)" },
# define VM_STORES_PARAM_INDEX		51
    { "void", "(i8*, i8)" },
# define VM_STORES_LOCAL_INDEX		52
    { "void", "(i8*, i8)" },
# define VM_STORES_GLOBAL_INDEX		53
    { "void", "(i8*, i16, i8)" },
# define VM_STORES_INDEX_INDEX		54
    { "void", "(i8*)" },
# define VM_DIV_INT			55
    { Int, "(i8*, " Int ", " Int ")" },
# define VM_LSHIFT_INT			56
    { Int, "(i8*, " Int ", " Int ")" },
# define VM_MOD_INT			57
    { Int, "(i8*, " Int ", " Int ")" },
# define VM_RSHIFT_INT			58
    { Int, "(i8*, " Int ", " Int ")" },
# define VM_TOFLOAT			59
    { Double, "(i8*)" },
# define VM_TOINT			60
    { Int, "(i8*)" },
# define VM_TOINT_FLOAT			61
    { Int, "(i8*, " Double ")" },
# define VM_NIL				62
    { "void", "(i8*)" },
# define VM_ADD_FLOAT			63
    { Double, "(i8*, " Double ", " Double ")" },
# define VM_DIV_FLOAT			64
    { Double, "(i8*, " Double ", " Double ")" },
# define VM_MULT_FLOAT			65
    { Double, "(i8*, " Double ", " Double ")" },
# define VM_SUB_FLOAT			66
    { Double, "(i8*, " Double ", " Double ")" },
# define VM_KFUNC			67
    { "void", "(i8*, i16, i32)" },
# define VM_KFUNC_INT			68
    { Int, "(i8*, i16, i32)" },
# define VM_KFUNC_FLOAT			69
    { Double, "(i8*, i16, i32)" },
# define VM_KFUNC_SPREAD		70
    { "void", "(i8*, i16, i32)" },
# define VM_KFUNC_SPREAD_INT		71
    { Int, "(i8*, i16, i32)" },
# define VM_KFUNC_SPREAD_FLOAT		72
    { Double, "(i8*, i16, i32)" },
# define VM_KFUNC_SPREAD_LVAL		73
    { "void", "(i8*, i16, i16, i32)" },
# define VM_DFUNC			74
    { "void", "(i8*, i16, i8, i32)" },
# define VM_DFUNC_INT			75
    { Int, "(i8*, i16, i8, i32)" },
# define VM_DFUNC_FLOAT			76
    { Double, "(i8*, i16, i8, i32)" },
# define VM_DFUNC_SPREAD		77
    { "void", "(i8*, i16, i8, i32)" },
# define VM_DFUNC_SPREAD_INT		78
    { Int, "(i8*, i16, i8, i32)" },
# define VM_DFUNC_SPREAD_FLOAT		79
    { Double, "(i8*, i16, i8, i32)" },
# define VM_FUNC			80
    { "void", "(i8*, i16, i32)" },
# define VM_FUNC_SPREAD			81
    { "void", "(i8*, i16, i32)" },
# define VM_POP				82
    { "void", "(i8*)" },
# define VM_POP_BOOL			83
    { "i1", "(i8*)" },
# define VM_POP_INT			84
    { Int, "(i8*)" },
# define VM_POP_FLOAT			85
    { Double, "(i8*)" },
# define VM_SWITCH_INT			86
    { "i1", "(i8*)" },
# define VM_SWITCH_RANGE		87
    { "i32", "(" Int "*, i32, " Int ")" },
# define VM_SWITCH_STRING		88
    { "i32", "(i8*, i16*, i32)" },
# define VM_RLIMITS			89
    { "void", "(i8*, i1)" },
# define VM_RLIMITS_END			90
    { "void", "(i8*)" },
# define VM_CATCH			91
    { "i8*", "(i8*)" },
# define VM_CAUGHT			92
    { "void", "(i8*, i1)" },
# define VM_CATCH_END			93
    { "void", "(i8*, i1)" },
# define VM_LINE			94
    { "void", "(i8*, i16)" },
# define VM_LOOP_TICKS			95
    { "void", "(i8*)" },
# define VM_FABS			96
    { Double, "(i8*, " Double ")" },
# define VM_FLOOR			97
    { Double, "(i8*, " Double ")" },
# define VM_CEIL			98
    { Double, "(i8*, " Double ")" },
# define VM_FMOD			99
    { Double, "(i8*, " Double ", " Double ")" },
# define VM_LDEXP			100
    { Double, "(i8*, " Double ", " Int ")" },
# define VM_EXP				101
    { Double, "(i8*, " Double ")" },
# define VM_LOG				102
    { Double, "(i8*, " Double ")" },
# define VM_LOG10			103
    { Double, "(i8*, " Double ")" },
# define VM_POW				104
    { Double, "(i8*, " Double ", " Double ")" },
# define VM_SQRT			105
    { Double, "(i8*, " Double ")" },
# define VM_COS				106
    { Double, "(i8*, " Double ")" },
# define VM_SIN				107
    { Double, "(i8*, " Double ")" },
# define VM_TAN				108
    { Double, "(i8*, " Double ")" },
# define VM_ACOS			109
    { Double, "(i8*, " Double ")" },
# define VM_ASIN			110
    { Double, "(i8*, " Double ")" },
# define VM_ATAN			111
    { Double, "(i8*, " Double ")" },
# define VM_ATAN2			112
    { Double, "(i8*, " Double ", " Double ")" },
# define VM_COSH			113
    { Double, "(i8*, " Double ")" },
# define VM_SINH			114
    { Double, "(i8*, " Double ")" },
# define VM_TANH			115
    { Double, "(i8*, " Double ")" },
# define VM_FUNCTIONS			116
};

class GenContext : public FlowContext {
public:
    GenContext(FILE *stream, CodeFunction *func, StackSize size, int num) :
	FlowContext(func, size), stream(stream), num(num) {
	block = NULL;
	next = 0;
	line = 0;
	storeSkip = false;
	switchList = NULL;
	count = 0;
    }

    virtual ~GenContext() { }

    /*
     * prepare context for block
     */
    void prepareGen(class Block *b) {
	block = b;
	if (b->nTo != 0) {
	    next = b->to[0]->first->addr;
	}
    }

    /*
     * generate reference
     */
    char *genRef() {
	static char bufs[3][10];
	static int n = 0;

	n = (n + 1) % 3;
	sprintf(bufs[n], "%%c%d", ++count);
	return bufs[n];
    }

    /*
     *generate floating point constant
     */
    char *genFloat(long double d) {
	static char buf[24];
	int sign, e;
# if DOUBLE_SIZE == 10
	uint32_t l1, l2;

	if (d == 0.0) {
	    sign = 0;
	    e = 0;
	    l1 = l2 = 0;
	} else {
	    sign = (d < 0.0);
	    d = frexpl(fabsl(d), &e);
	    e += 0x3ffe;
	    d = ldexpl(d, 32);
	    l1 = (uint32_t) d;
	    l2 = (uint32_t) ldexpl(modfl(d, &d), 32);
	}
	sprintf(buf, "0xK%04x%08lx%08lx", (sign << 15) + e, (unsigned long) l1,
		(unsigned long) l2);
# else
	unsigned long long l;

	if (d == 0.0) {
	    sign = 0;
	    e = 0;
	    l = 0;
	} else {
	    sign = (d < 0.0);
	    d = frexpl(fabsl(d), &e);
	    e += 0x3fe;
	    l = ((unsigned long long) ldexpl(d, 53)) & 0x000fffffffffffffLL;
	}
	sprintf(buf, "0x%03x%013llx", (sign << 11) + e, l);
# endif
	return buf;
    }

    /*
     * copy integer value
     */
    void copyInt(char *to, char *from) {
	fprintf(stream, "\t%s = xor " Int " %s, 0\n", to, from);
    }

    /*
     * copy float value
     */
    void copyFloat(char *to, char *from) {
	fprintf(stream, "\t%s = fadd fast " Double " %s, %s\n", to, from,
		genFloat(0.0L));
    }

    /*
     * load a function address
     */
    char *load(int func) {
	char *ref;

	ref = genRef();
	fprintf(stream,
		"\t%sg = getelementptr inbounds "
# ifndef LLVM3_6
						"i8*, "
# endif
						      "i8** %%vmtab, i32 %d\n",
		ref, func);
	fprintf(stream, "\t%sl = load "
# ifndef LLVM3_6
				      "i8*, "
# endif
					    "i8** %sg, align %d\n",
		ref, ref, (int) sizeof(void *));
	fprintf(stream, "\t%s = bitcast i8* %sl to %s %s*\n",
		ref, ref, functions[func].ret, functions[func].args);
	return ref;
    }

    /*
     * call VM function without arguments
     */
    void call(int func, char *ref) {
	fprintf(stream, "\t%s = call %s %s(i8* %%f)\n", ref,
		functions[func].ret, load(func));
    }

    /*
     * call VM function with arguments
     */
    void callArgs(int func, char *ref) {
	fprintf(stream, "\t%s = call %s %s(i8* %%f, ", ref,
		functions[func].ret, load(func));
    }

    /*
     * void call VM function without arguments
     */
    void voidCall(int func) {
	fprintf(stream, "\tcall %s %s(i8* %%f)\n", functions[func].ret,
		load(func));
    }

    /*
     * void call VM function with arguments
     */
    void voidCallArgs(int func) {
	fprintf(stream, "\tcall %s %s(i8* %%f, ", functions[func].ret,
		load(func));
    }

    /*
     * possibly skip stores
     */
    void skip(bool flag) {
	storeSkip = flag;
    }

    /*
     * skipping stores?
     */
    bool skipping() {
	return storeSkip;
    }

    /*
     * clean up after STORES
     */
    void popStores(char *ref, Type type) {
	voidCall(VM_POP);
	if (storePop() != NULL) {
	    voidCall(VM_POP);
	} else if (type == LPC_TYPE_INT) {
	    call(VM_POP_INT, ref);
	} else if (type == LPC_TYPE_FLOAT) {
	    call(VM_POP_FLOAT, ref);
	}
    }

    /*
     * relay needed between two blocks?
     */
    static bool relay(Block *from, Block *to) {
	return (from->first->addr >= to->first->addr ||
	        ((from->flags & BLOCK_SWITCHINT) &&
		 (to->flags & BLOCK_DEFAULT)));
    }

    /*
     * block exit label
     */
    static char *label(Block *from, Block *to) {
	static char buf[12];

	if (to != NULL && relay(from, to)) {
	    sprintf(buf, "L%04xT%04x", from->first->addr, to->first->addr);
	} else {
	    sprintf(buf, "L%04x", from->first->addr);
	}

	return buf;
    }

    /*
     * default block exit label
     */
    char *label(Block *to) {
	return label(block, to);
    }

    /*
     * default block target label
     */
    char *target(Block *to) {
	static char buf[12];

	if (relay(block, to)) {
	    sprintf(buf, "L%04xT%04x", block->first->addr, to->first->addr);
	} else {
	    sprintf(buf, "L%04x", to->first->addr);
	}

	return buf;
    }

    /*
     * jump relay
     */
    void jumpRelay(Block *to) {
	if (relay(block, to)) {
	    fprintf(stream, "%s:\n", target(to));
	    if (block->first->addr >= to->first->addr) {
		voidCall(VM_LOOP_TICKS);
	    }
	    fprintf(stream, "\tbr label %%L%04x\n", to->first->addr);
	}
    }

    /*
     * store local variables
     */
    void saveLocals() {
	LPCLocal n;

	for (n = 0; n < nLocals; n++) {
	    if (block->localOut(n) != 0) {
		switch (block->localType(n)) {
		case LPC_TYPE_INT:
		    voidCallArgs(VM_STORE_LOCAL_INT);
		    fprintf(stream, "i8 %u, " Int " %s)\n", n + 1,
			    ClangCode::localRef(n, block->localOut(n)));
		    break;

		case LPC_TYPE_FLOAT:
		    voidCallArgs(VM_STORE_LOCAL_FLOAT);
		    fprintf(stream, "i8 %u, " Double " %s)\n", n + 1,
			    ClangCode::localRef(n, block->localOut(n)));
		    break;
		}
	    }
	}
    }

    /*
     * store local variables that are merged to LPC_TYPE_MIXED in a followup
     */
    void saveBeforeMerge(Block *b) {
	LPCLocal n;
	CodeSize i;

	for (n = 0; n < nLocals; n++) {
	    if (b->localOut(n) != 0) {
		switch (b->localType(n)) {
		case LPC_TYPE_INT:
		    for (i = 0; i < b->nTo; i++) {
			if (ClangBlock::mergedLocalType(b->to[i], n) ==
							    LPC_TYPE_MIXED) {
			    voidCallArgs(VM_STORE_LOCAL_INT);
			    fprintf(stream, "i8 %u, " Int " %s)\n", n + 1,
				    ClangCode::localRef(n, b->localOut(n)));
			    break;
			}
		    }
		    break;

		case LPC_TYPE_FLOAT:
		    for (i = 0; i < b->nTo; i++) {
			if (ClangBlock::mergedLocalType(b->to[i], n) ==
							    LPC_TYPE_MIXED) {
			    voidCallArgs(VM_STORE_LOCAL_FLOAT);
			    fprintf(stream, "i8 %u, " Double " %s)\n", n + 1,
				    ClangCode::localRef(n, b->localOut(n)));
			    break;
			}
		    }
		    break;
		}
	    }
	}
    }

    FILE *stream;		/* output file */
    int num;			/* function number */
    Block *block;		/* current block */
    CodeSize next;		/* address of next block */
    CodeLine line;		/* current line number */
    bool storeSkip;		/* skipping stores? */
    ClangCode *switchList;	/* list of switch tables */
    int count;			/* reference counter */
};


ClangCode::ClangCode(CodeFunction *function) :
    FlowCode(function)
{
    list = NULL;
}

ClangCode::~ClangCode()
{
}

/*
 * return a type if a value should be kept off the stack, LPC_TYPE_NIL otherwise
 */
Type ClangCode::offStack(GenContext *context, StackSize sp)
{
    Type type;
    Code *code;

    type = context->get(sp).type;
    code = context->consumer(sp, type);
    if (code == NULL) {
	return LPC_TYPE_NIL;
    }

    switch (code->instruction) {
    case STORE_PARAM:
    case STORE_LOCAL:
    case STORE_GLOBAL:
	if (type == LPC_TYPE_INT || type == LPC_TYPE_FLOAT) {
	    return type;
	}
	break;

    case JUMP_ZERO:
    case JUMP_NONZERO:
    case SWITCH_INT:
    case SWITCH_RANGE:
	if (type == LPC_TYPE_INT) {
	    return type;
	}
	break;

    case KFUNC:
	switch (code->kfun.func) {
	case KF_ADD_INT:
	case KF_ADD1_INT:
	case KF_AND_INT:
	case KF_DIV_INT:
	case KF_EQ_INT:
	case KF_GE_INT:
	case KF_GT_INT:
	case KF_LE_INT:
	case KF_LSHIFT_INT:
	case KF_LT_INT:
	case KF_MOD_INT:
	case KF_MULT_INT:
	case KF_NE_INT:
	case KF_NEG_INT:
	case KF_NOT_INT:
	case KF_OR_INT:
	case KF_RSHIFT_INT:
	case KF_SUB_INT:
	case KF_SUB1_INT:
	case KF_TST_INT:
	case KF_UMIN_INT:
	case KF_XOR_INT:
	    return LPC_TYPE_INT;

	case KF_ADD_FLT:
	case KF_ADD1_FLT:
	case KF_DIV_FLT:
	case KF_EQ_FLT:
	case KF_GE_FLT:
	case KF_GT_FLT:
	case KF_LE_FLT:
	case KF_LT_FLT:
	case KF_MULT_FLT:
	case KF_NE_FLT:
	case KF_NOT_FLT:
	case KF_SUB_FLT:
	case KF_SUB1_FLT:
	case KF_TST_FLT:
	case KF_UMIN_FLT:
	case KF_FABS:
	case KF_FLOOR:
	case KF_CEIL:
	case KF_FMOD:
	case KF_EXP:
	case KF_LOG:
	case KF_LOG10:
	case KF_POW:
	case KF_SQRT:
	case KF_COS:
	case KF_SIN:
	case KF_TAN:
	case KF_ACOS:
	case KF_ASIN:
	case KF_ATAN:
	case KF_ATAN2:
	case KF_COSH:
	case KF_SINH:
	case KF_TANH:
	    return LPC_TYPE_FLOAT;

	case KF_TOFLOAT:
	case KF_TOINT:
	case KF_LDEXP:
	    if (type == LPC_TYPE_INT || type == LPC_TYPE_FLOAT) {
		return type;
	    }
	    break;
	}
	break;

    default:
	break;
    }

    return LPC_TYPE_NIL;
}

/*
 * obtain an argument which the LPC compiler knew to be an integer
 */
void ClangCode::intArg(GenContext *context, StackSize sp)
{
    if (context->get(sp).type != LPC_TYPE_INT) {
	context->call(VM_POP_INT, tmpRef(sp));
    }
}

/*
 * obtain an argument which the LPC compiler knew to be a float
 */
void ClangCode::floatArg(GenContext *context, StackSize sp)
{
    if (context->get(sp).type != LPC_TYPE_FLOAT) {
	context->call(VM_POP_FLOAT, tmpRef(sp));
    }
}

/*
 * return a name for a reference to a temporary value
 */
char *ClangCode::tmpRef(StackSize sp)
{
    static char bufs[3][10];
    static int n = 0;

    n = (n + 1) % 3;
    sprintf(bufs[n], "%%t%d", sp);
    return bufs[n];
}

/*
 * return a name for a parameter reference
 */
char *ClangCode::paramRef(LPCParam param, int ref)
{
    static char buf[20];

    if (ref < 0) {
	/* merged */
	sprintf(buf, "%%p%um%d", param, -ref);
    } else {
	if (ref == GenContext::INITIAL) {
	    ref = 0;
	}
	sprintf(buf, "%%p%ur%d", param, ref);
    }
    return buf;
}

/*
 * return a name for a parameter phi reference
 */
char *ClangCode::paramPhi(LPCParam param, int ref)
{
    static char buf[20];

    if (ref < 0) {
	/* merged */
	sprintf(buf, "%%p%um%dx", param, -ref);
    } else {
	if (ref == GenContext::INITIAL) {
	    ref = 0;
	}
	sprintf(buf, "%%p%ur%dx", param, ref);
    }
    return buf;
}

/*
 * return a name for a local var reference
 */
char *ClangCode::localRef(LPCLocal local, int ref)
{
    static char buf[20];

    if (ref < 0) {
	/* merged */
	sprintf(buf, "%%l%um%d", local, -ref);
    } else {
	if (ref == GenContext::INITIAL) {
	    ref = 0;
	}
	sprintf(buf, "%%l%ur%d", local, ref);
    }
    return buf;
}

/*
 * return a name for a local var phi reference
 */
char *ClangCode::localPhi(LPCLocal local, int ref)
{
    static char buf[20];

    if (ref < 0) {
	/* merged */
	sprintf(buf, "%%l%um%dx", local, -ref);
    } else {
	if (ref == GenContext::INITIAL) {
	    ref = 0;
	}
	sprintf(buf, "%%l%ur%dx", local, ref);
    }
    return buf;
}

/*
 * return a name for the parameter this Code refers to
 */
char *ClangCode::paramRef(GenContext *context, LPCParam param)
{
    int ref;

    ref = outputRef();
    if (ref == 0) {
	ref = context->block->paramIn(param);
    }
    return paramRef(param, ref);
}

/*
 * return a name for the local var this Code refers to
 */
char *ClangCode::localRef(GenContext *context, LPCLocal local)
{
    int ref;

    ref = outputRef();
    if (ref == 0) {
	ref = context->block->localIn(local);
    }
    return localRef(local, ref);
}

/*
 * return a name for the local input var this Code refers to
 */
char *ClangCode::localPre(GenContext *context, LPCLocal local)
{
    int ref;

    ref = inputRef();
    if (ref == 0) {
	ref = context->block->localIn(local);
    }
    return localRef(local, ref);
}

/*
 * push current result on the stack, if needed
 */
void ClangCode::result(GenContext *context)
{
    StackSize sp;

    sp = stackPointer();
    if (!pop && offStack(context, sp) == LPC_TYPE_NIL) {
	if (context->get(sp).type == LPC_TYPE_INT) {
	    context->voidCallArgs(VM_INT);
	    fprintf(context->stream, Int " %s)\n", tmpRef(sp));
	} else {
	    context->voidCallArgs(VM_FLOAT);
	    fprintf(context->stream, Double " %s)\n", tmpRef(sp));
	}
    }
    context->sp = sp;
}

/*
 * obtain the argument to an int/range switch, and branch to default when
 * it isn't an int
 */
void ClangCode::switchInt(GenContext *context)
{
    char *ref;

    if (context->get(context->sp).type != LPC_TYPE_INT) {
	context->block->flags |= BLOCK_SWITCHINT;
	ref = context->genRef();
	context->call(VM_SWITCH_INT, ref);
	fprintf(context->stream, "\tbr i1 %s, label %%%sS, label %%%s\n", ref,
		context->label(NULL),
		context->target(context->block->to[0]));
	fprintf(context->stream, "%sS:\n", context->label(NULL));
	context->call(VM_POP_INT, tmpRef(context->sp));
    }
}

/*
 * reference a switch table, to be generated
 */
void ClangCode::genTable(GenContext *context, const char *type)
{
    fprintf(context->stream, "%s* getelementptr inbounds ("
# ifndef LLVM3_6
							  "[%d x %s], "
# endif
	    "[%d x %s]* @func%d.%04x, i32 0, i32 0), i32 %u", type,
# ifndef LLVM3_6
	    2 * (size - 1), type,
# endif
	    2 * (size - 1), type, context->num, addr, size - 1);

    /* add myself to the switch table list */
    list = context->switchList;
    context->switchList = this;
}

/*
 * emit instructions for this Code
 */
void ClangCode::emit(GenContext *context)
{
    StackSize sp;
    long double d;
    char *ref, *ref2;
    int i;

    if (line != context->line) {
	context->voidCallArgs(VM_LINE);
	fprintf(context->stream, "i16 %u)\n", line);
	context->line = line;
    }

    sp = stackPointer();
    switch (instruction) {
    case INT:
	fprintf(context->stream, "\t%s = xor " Int " %lld, 0\n", tmpRef(sp),
		(long long) num);
	result(context);
	return;

    case FLOAT:
	if ((flt.high | flt.low) == 0) {
	    d = 0.0;
	} else {
	    d = ldexpl((long double) (0x10000 | (flt.high & 0xffff)), 64);
	    d = ldexpl(d + flt.low, ((flt.high >> 16) & 0x7fff) - 0x404f);
	    if (flt.high & 0x80000000) {
		d = -d;
	    }
	}
	fprintf(context->stream, "\t%s = fadd fast " Double " %s, ", tmpRef(sp),
		context->genFloat(d));
	fprintf(context->stream, "%s\n", context->genFloat(0.0L));
	result(context);
	return;

    case STRING:
	context->voidCallArgs(VM_STRING);
	fprintf(context->stream, "i16 %u, i16 %u)\n", str.inherit, str.index);
	break;

    case PARAM:
	switch (context->get(sp).type) {
	case LPC_TYPE_INT:
	    context->copyInt(tmpRef(sp), paramRef(context, param));
	    result(context);
	    return;

	case LPC_TYPE_FLOAT:
	    context->copyFloat(tmpRef(sp), paramRef(context, param));
	    result(context);
	    return;

	default:
	    switch (offStack(context, sp)) {
	    case LPC_TYPE_INT:
		context->callArgs(VM_PARAM_INT, tmpRef(sp));
		fprintf(context->stream, "i8 %u)\n", param);
		break;

	    case LPC_TYPE_FLOAT:
		context->callArgs(VM_PARAM_FLOAT, tmpRef(sp));
		fprintf(context->stream, "i8 %u)\n", param);
		break;

	    default:
		context->voidCallArgs(VM_PARAM);
		fprintf(context->stream, "i8 %u)\n", param);
		break;
	    }
	    break;
	}
	break;

    case LOCAL:
	switch (context->get(sp).type) {
	case LPC_TYPE_INT:
	    context->copyInt(tmpRef(sp), localRef(context, local));
	    result(context);
	    return;

	case LPC_TYPE_FLOAT:
	    context->copyFloat(tmpRef(sp), localRef(context, local));
	    result(context);
	    return;

	default:
	    switch (offStack(context, sp)) {
	    case LPC_TYPE_INT:
		context->callArgs(VM_LOCAL_INT, tmpRef(sp));
		fprintf(context->stream, "i8 %u)\n", local + 1);
		break;

	    case LPC_TYPE_FLOAT:
		context->callArgs(VM_LOCAL_FLOAT, tmpRef(sp));
		fprintf(context->stream, "i8 %u)\n", local + 1);
		break;

	    default:
		context->voidCallArgs(VM_LOCAL);
		fprintf(context->stream, "i8 %u)\n", local + 1);
		break;
	    }
	    break;
	}
	break;

    case GLOBAL:
	switch (offStack(context, sp)) {
	case LPC_TYPE_INT:
	    context->callArgs(VM_GLOBAL_INT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i8 %u)\n", var.inherit,
		    var.index);
	    result(context);
	    return;

	case LPC_TYPE_FLOAT:
	    context->callArgs(VM_GLOBAL_FLOAT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i8 %u)\n", var.inherit,
		    var.index);
	    result(context);
	    return;

	default:
	    context->voidCallArgs(VM_GLOBAL);
	    fprintf(context->stream, "i16 %u, i8 %u)\n", var.inherit,
		    var.index);
	    break;
	}
	break;

    case INDEX:
	if (offStack(context, sp) == LPC_TYPE_INT) {
	    context->call(VM_INDEX_INT, tmpRef(sp));
	    result(context);
	    return;
	}
	context->voidCall(VM_INDEX);
	break;

    case INDEX2:
	if (offStack(context, sp) == LPC_TYPE_INT) {
	    context->call(VM_INDEX2_INT, tmpRef(sp));
	    result(context);
	    return;
	}
	context->voidCall(VM_INDEX2);
	break;

    case AGGREGATE:
	context->voidCallArgs(VM_AGGREGATE);
	fprintf(context->stream, "i16 %u)\n", size);
	break;

    case MAP_AGGREGATE:
	context->voidCallArgs(VM_MAP_AGGREGATE);
	fprintf(context->stream, "i16 %u)\n", size);
	break;

    case CAST:
	switch (type.type) {
	case LPC_TYPE_INT:
	    context->call(VM_CAST_INT, tmpRef(sp));
	    result(context);
	    return;

	case LPC_TYPE_FLOAT:
	    context->call(VM_CAST_FLOAT, tmpRef(sp));
	    result(context);
	    return;

	case LPC_TYPE_CLASS:
	    context->voidCallArgs(VM_CAST);
	    fprintf(context->stream, "i8 %u, i16 %u, i16 %u)\n", type.type,
		    type.inherit, type.index);
	    break;

	default:
	    context->voidCallArgs(VM_CAST);
	    fprintf(context->stream, "i8 %u, i16 0, i16 0)\n", type.type);
	    break;
	}
	break;

    case INSTANCEOF:
	context->callArgs(VM_INSTANCEOF, tmpRef(sp));
	fprintf(context->stream, "i16 %u, i16 %u)\n", str.inherit, str.index);
	result(context);
	return;

    case CHECK_RANGE:
	context->voidCall(VM_RANGE);
	break;

    case CHECK_RANGE_FROM:
	context->voidCall(VM_RANGE_FROM);
	break;

    case CHECK_RANGE_TO:
	context->voidCall(VM_RANGE_TO);
	break;

    case STORE_PARAM:
	switch (context->get(context->sp).type) {
	case LPC_TYPE_INT:
	    context->copyInt(paramRef(context, param), tmpRef(context->sp));
	    context->voidCallArgs(VM_STORE_PARAM_INT);
	    fprintf(context->stream, "i8 %u, " Int " %s)\n", param,
		    tmpRef(context->sp));
	    if (!pop) {
		context->copyInt(tmpRef(sp), tmpRef(context->sp));
	    }
	    result(context);
	    return;

	case LPC_TYPE_FLOAT:
	    context->copyFloat(paramRef(context, param), tmpRef(context->sp));
	    context->voidCallArgs(VM_STORE_PARAM_FLOAT);
	    fprintf(context->stream, "i8 %u, " Double " %s)\n", param,
		    tmpRef(context->sp));
	    if (!pop) {
		context->copyFloat(tmpRef(sp), tmpRef(context->sp));
	    }
	    result(context);
	    return;

	default:
	    context->voidCallArgs(VM_STORE_PARAM);
	    fprintf(context->stream, "i8 %u)\n", param);
	    break;
	}
	break;

    case STORE_LOCAL:
	switch (context->get(context->sp).type) {
	case LPC_TYPE_INT:
	    context->copyInt(localRef(context, local), tmpRef(context->sp));
	    if (context->level > 0) {
		context->voidCallArgs(VM_STORE_LOCAL_INT);
		fprintf(context->stream, "i8 %u, " Int " %s)\n", local + 1,
			tmpRef(context->sp));
	    }
	    if (!pop) {
		context->copyInt(tmpRef(sp), tmpRef(context->sp));
	    }
	    result(context);
	    return;

	case LPC_TYPE_FLOAT:
	    context->copyFloat(localRef(context, local), tmpRef(context->sp));
	    if (context->level > 0) {
		context->voidCallArgs(VM_STORE_LOCAL_FLOAT);
		fprintf(context->stream, "i8 %u, " Double " %s)\n", local + 1,
			tmpRef(context->sp));
	    }
	    if (!pop) {
		context->copyFloat(tmpRef(sp), tmpRef(context->sp));
	    }
	    result(context);
	    return;

	default:
	    context->voidCallArgs(VM_STORE_LOCAL);
	    fprintf(context->stream, "i8 %u)\n", local + 1);
	    break;
	}
	break;

    case STORE_GLOBAL:
	switch (context->get(context->sp).type) {
	case LPC_TYPE_INT:
	    context->voidCallArgs(VM_STORE_GLOBAL_INT);
	    fprintf(context->stream, "i16 %u, i8 %u, " Int " %s)\n",
		    var.inherit, var.index, tmpRef(context->sp));
	    if (!pop) {
		context->copyInt(tmpRef(sp), tmpRef(context->sp));
	    }
	    result(context);
	    return;

	case LPC_TYPE_FLOAT:
	    context->voidCallArgs(VM_STORE_GLOBAL_FLOAT);
	    fprintf(context->stream, "i16 %u, i8 %u, " Double " %s)\n",
		    var.inherit, var.index, tmpRef(context->sp));
	    if (!pop) {
		context->copyFloat(tmpRef(sp), tmpRef(context->sp));
	    }
	    result(context);
	    return;

	default:
	    context->voidCallArgs(VM_STORE_GLOBAL);
	    fprintf(context->stream, "i16 %u, i8 %u)\n", var.inherit,
		    var.index);
	    break;
	}
	break;

    case STORE_INDEX:
	context->voidCall(VM_STORE_INDEX);
	break;

    case STORE_PARAM_INDEX:
	context->voidCallArgs(VM_STORE_PARAM_INDEX);
	fprintf(context->stream, "i8 %u)\n", param);
	break;

    case STORE_LOCAL_INDEX:
	context->voidCallArgs(VM_STORE_LOCAL_INDEX);
	fprintf(context->stream, "i8 %u)\n", local + 1);
	break;

    case STORE_GLOBAL_INDEX:
	context->voidCallArgs(VM_STORE_GLOBAL_INDEX);
	fprintf(context->stream, "i16 %u, i8 %u)\n", var.inherit, var.index);
	break;

    case STORE_INDEX_INDEX:
	context->voidCall(VM_STORE_INDEX_INDEX);
	break;

    case STORES:
	if (context->stores(size, NULL)) {
	    context->skip(false);
	    context->voidCallArgs(VM_STORES);
	    fprintf(context->stream, "i8 %u)\n", size);
	} else {
	    context->voidCall(VM_POP);
	}
	break;

    case STORES_LVAL:
	if (context->stores(size, (pop) ? this : NULL)) {
	    context->skip(true);
	    if (next->instruction == STORES_SPREAD) {
		context->voidCallArgs(VM_STORES_SPREAD);
		fprintf(context->stream, "i8 %u, ", size);
	    } else {
		context->voidCallArgs(VM_STORES_LVAL);
		fprintf(context->stream, "i8 %u)\n", size);
	    }
	} else {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	context->sp = sp;
	return;

    case STORES_SPREAD:
	if (type.type == LPC_TYPE_CLASS) {
	    fprintf(context->stream, "i8 %u, i8 %u, i16 %u, i16 %u)\n",
		    spread, type.type, type.inherit, type.index);
	} else {
	    fprintf(context->stream, "i8 %u, i8 %u, i16 0, i16 0)\n", spread,
		    type.type);
	}
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_CAST:
	context->voidCallArgs(VM_STORES_CAST);
	if (type.type == LPC_TYPE_CLASS) {
	    fprintf(context->stream, "i8 %u, i16 %u, i16 %u)\n", type.type,
		    type.inherit, type.index);
	} else {
	    fprintf(context->stream, "i8 %u, i16 0, i16 0)\n", type.type);
	}
	context->castType = simplifiedType(type.type);
	break;

    case STORES_PARAM:
	switch (context->castType) {
	case LPC_TYPE_INT:
	    context->callArgs(VM_STORES_PARAM_INT, paramRef(context, param));
	    fprintf(context->stream, "i8 %u)\n", param);
	    break;

	case LPC_TYPE_FLOAT:
	    context->callArgs(VM_STORES_PARAM_FLOAT, paramRef(context, param));
	    fprintf(context->stream, "i8 %u)\n", param);
	    break;

	default:
	    context->voidCallArgs(VM_STORES_PARAM);
	    fprintf(context->stream, "i8 %u)\n", param);
	    break;
	}
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_LOCAL:
	switch (context->castType) {
	case LPC_TYPE_INT:
	    context->callArgs(VM_STORES_LOCAL_INT, localRef(context, local));
	    fprintf(context->stream, "i8 %u, " Int " %s)\n", local + 1,
		    (context->skipping()) ? localPre(context, local) : "0");
	    break;

	case LPC_TYPE_FLOAT:
	    context->callArgs(VM_STORES_LOCAL_FLOAT, localRef(context, local));
	    fprintf(context->stream, "i8 %u, " Double " %s)\n", local + 1,
		    (context->skipping()) ?
		     localPre(context, local) : context->genFloat(0.0L));
	    break;

	default:
	    context->voidCallArgs(VM_STORES_LOCAL);
	    fprintf(context->stream, "i8 %u)\n", local + 1);
	    break;
	}
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_GLOBAL:
	context->voidCallArgs(VM_STORES_GLOBAL);
	fprintf(context->stream, "i16 %u, i8 %u)\n", var.inherit, var.index);
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_INDEX:
	context->voidCall(VM_STORES_INDEX);
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_PARAM_INDEX:
	context->voidCallArgs(VM_STORES_PARAM_INDEX);
	fprintf(context->stream, "i8 %u)\n", param);
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_LOCAL_INDEX:
	context->voidCallArgs(VM_STORES_LOCAL_INDEX);
	fprintf(context->stream, "i8 %u)\n", local + 1);
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_GLOBAL_INDEX:
	context->voidCallArgs(VM_STORES_GLOBAL_INDEX);
	fprintf(context->stream, "i16 %u, i8 %u)\n", var.inherit, var.index);
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case STORES_INDEX_INDEX:
	context->voidCall(VM_STORES_INDEX_INDEX);
	if (!context->storeN()) {
	    context->popStores(tmpRef(sp), offStack(context, sp));
	}
	break;

    case JUMP:
	break;

    case JUMP_ZERO:
	ref = context->genRef();
	if (context->get(context->sp).type == LPC_TYPE_INT) {
	    fprintf(context->stream, "\t%s = icmp ne " Int " %s, 0\n", ref,
		    tmpRef(context->sp));
	} else {
	    context->call(VM_POP_BOOL, ref);
	}
	fprintf(context->stream, "\tbr i1 %s, label %%L%04x, label %%%s\n",
		ref, context->next, context->target(context->block->to[1]));
	context->jumpRelay(context->block->to[1]);
	context->sp = sp;
	return;

    case JUMP_NONZERO:
	ref = context->genRef();
	if (context->get(context->sp).type == LPC_TYPE_INT) {
	    fprintf(context->stream, "\t%s = icmp ne " Int " %s, 0\n", ref,
		    tmpRef(context->sp));
	} else {
	    context->call(VM_POP_BOOL, ref);
	}
	fprintf(context->stream, "\tbr i1 %s, label %%%s, label %%L%04x\n",
		ref, context->target(context->block->to[1]), context->next);
	context->jumpRelay(context->block->to[1]);
	context->sp = sp;
	return;

    case SWITCH_INT:
	switchInt(context);
	if (size > 1) {
	    fprintf(context->stream, "\tswitch " Int " %s, label %%%s [\n",
		    tmpRef(context->sp),
		    context->target(context->block->to[0]));
	    for (i = 1; i < size; i++) {
		fprintf(context->stream, "\t\t" Int " %lld, label %%%s\n",
			(long long) caseInt[i].num,
			context->target(context->block->to[i]));
	    }
	    fprintf(context->stream, "\t]\n");
	} else {
	    fprintf(context->stream, "\tbr label %%%s\n",
		    context->target(context->block->to[0]));
	}
	for (i = 0; i < size; i++) {
	    context->jumpRelay(context->block->to[i]);
	}
	context->sp = sp;
	return;

    case SWITCH_RANGE:
	switchInt(context);
	if (size > 1) {
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = call i32 %s(", ref,
		    context->load(VM_SWITCH_RANGE));
	    genTable(context, Int);
	    fprintf(context->stream,
		    ", " Int " %s)\n\tswitch i32 %s, label %%%s [\n",
		    tmpRef(context->sp), ref,
		    context->target(context->block->to[0]));
	    for (i = 1; i < size; i++) {
		fprintf(context->stream, "\t\ti32 %d, label %%%s\n", i - 1,
			context->target(context->block->to[i]));
	    }
	    fprintf(context->stream, "\t]\n");
	} else {
	    fprintf(context->stream, "\tbr label %%%s\n",
		    context->target(context->block->to[0]));
	}
	for (i = 0; i < size; i++) {
	    context->jumpRelay(context->block->to[i]);
	}
	context->sp = sp;
	return;

    case SWITCH_STRING:
	if (size > 1) {
	    ref = context->genRef();
	    context->callArgs(VM_SWITCH_STRING, ref);
	    genTable(context, "i16");
	    fprintf(context->stream, ")\n\tswitch i32 %s, label %%%s [\n",
		    ref, context->target(context->block->to[0]));
	    for (i = 1; i < size; i++) {
		fprintf(context->stream, "\t\ti32 %d, label %%%s\n", i - 1,
			context->target(context->block->to[i]));
	    }
	    fprintf(context->stream, "\t]\n");
	} else {
	    context->voidCall(VM_POP);
	    fprintf(context->stream, "\tbr label %%%s\n",
		    context->target(context->block->to[0]));
	}
	for (i = 0; i < size; i++) {
	    context->jumpRelay(context->block->to[i]);
	}
	context->sp = sp;
	return;

    case KFUNC:
	switch (kfun.func) {
	case KF_ADD_INT:
	    fprintf(context->stream, "\t%s = add nsw " Int " %s, %s\n",
		    tmpRef(sp), tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_ADD1_INT:
	    fprintf(context->stream, "\t%s = add nsw " Int " %s, 1\n",
		    tmpRef(sp), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_AND_INT:
	    fprintf(context->stream, "\t%s = and " Int " %s, %s\n",
		    tmpRef(sp), tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_DIV_INT:
	    context->callArgs(VM_DIV_INT, tmpRef(sp));
	    fprintf(context->stream, Int " %s, " Int " %s)\n",
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_EQ_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp eq " Int " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_GE_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp sge " Int " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_GT_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp sgt " Int " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_LE_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp sle " Int " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_LSHIFT_INT:
	    context->callArgs(VM_LSHIFT_INT, tmpRef(sp));
	    fprintf(context->stream, Int " %s, " Int " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_LT_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp slt " Int " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_MOD_INT:
	    context->callArgs(VM_MOD_INT, tmpRef(sp));
	    fprintf(context->stream, Int " %s, " Int " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_MULT_INT:
	    fprintf(context->stream, "\t%s = mul nsw " Int " %s, %s\n",
		    tmpRef(sp), tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_NE_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp ne " Int " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_NEG_INT:
	    fprintf(context->stream, "\t%s = xor " Int " %s, -1\n",
		    tmpRef(sp), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_NOT_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp eq " Int " %s, 0\n", ref,
		    tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_OR_INT:
	    fprintf(context->stream, "\t%s = or " Int " %s, %s\n",
		    tmpRef(sp), tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_RSHIFT_INT:
	    context->callArgs(VM_RSHIFT_INT, tmpRef(sp));
	    fprintf(context->stream, Int " %s, " Int " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_SUB_INT:
	    fprintf(context->stream, "\t%s = sub nsw " Int " %s, %s\n",
		    tmpRef(sp), tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_SUB1_INT:
	    fprintf(context->stream, "\t%s = sub nsw " Int " %s, 1\n",
		    tmpRef(sp), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_TOFLOAT:
	    switch (context->get(context->sp).type) {
	    case LPC_TYPE_INT:
		fprintf(context->stream,
			"\t%s = sitofp " Int " %s to " Double "\n", tmpRef(sp),
			tmpRef(context->sp));
		break;

	    case LPC_TYPE_FLOAT:
		context->copyFloat(tmpRef(sp), tmpRef(context->sp));
		break;

	    default:
		context->call(VM_TOFLOAT, tmpRef(sp));
		break;
	    }
	    result(context);
	    return;

	case KF_TOINT:
	    switch (context->get(context->sp).type) {
	    case LPC_TYPE_INT:
		context->copyInt(tmpRef(sp), tmpRef(context->sp));
		break;

	    case LPC_TYPE_FLOAT:
		context->callArgs(VM_TOINT_FLOAT, tmpRef(sp));
		fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
		break;

	    default:
		context->call(VM_TOINT, tmpRef(sp));
		break;
	    }
	    result(context);
	    return;

	case KF_TST_INT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = icmp ne " Int " %s, 0\n", ref,
		    tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_UMIN_INT:
	    fprintf(context->stream, "\t%s = sub nsw " Int " 0, %s\n",
		    tmpRef(sp), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_XOR_INT:
	    fprintf(context->stream, "\t%s = xor " Int " %s, %s\n", tmpRef(sp),
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_NIL:
	    context->voidCall(VM_NIL);
	    break;

	case KF_ADD_FLT:
	    context->callArgs(VM_ADD_FLOAT, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Double " %s)\n",
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_ADD1_FLT:
	    fprintf(context->stream, "\t%s = fadd " Double " %s, %s\n",
		    tmpRef(sp), tmpRef(context->sp), context->genFloat(1.0L));
	    result(context);
	    return;

	case KF_DIV_FLT:
	    context->callArgs(VM_DIV_FLOAT, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Double " %s)\n",
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_EQ_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp oeq " Double " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_GE_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp oge " Double " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_GT_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp ogt " Double " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_LE_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp ole " Double " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_LT_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp olt " Double " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_MULT_FLT:
	    context->callArgs(VM_MULT_FLOAT, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Double " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_NE_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp one " Double " %s, %s\n", ref,
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_NOT_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp oeq " Double " %s, %s\n",
		    ref, tmpRef(context->sp), context->genFloat(0.0L));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_SUB_FLT:
	    context->callArgs(VM_SUB_FLOAT, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Double " %s)\n",
		    tmpRef(context->nextSp(context->sp)), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_SUB1_FLT:
	    fprintf(context->stream, "\t%s = fsub " Double " %s, %s\n",
		    tmpRef(sp), tmpRef(context->sp), context->genFloat(1.0L));
	    result(context);
	    return;

	case KF_TST_FLT:
	    ref = context->genRef();
	    fprintf(context->stream, "\t%s = fcmp one " Double " %s, %s\n",
		    ref, tmpRef(context->sp), context->genFloat(0.0L));
	    fprintf(context->stream, "\t%s = zext i1 %s to " Int "\n",
		    tmpRef(sp), ref);
	    result(context);
	    return;

	case KF_UMIN_FLT:
	    fprintf(context->stream, "\t%s = fsub " Double " %s, %s\n",
		    tmpRef(sp), context->genFloat(0.0L), tmpRef(context->sp));
	    result(context);
	    return;

	case KF_FABS:
	    context->callArgs(VM_FABS, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_FLOOR:
	    context->callArgs(VM_FLOOR, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_CEIL:
	    context->callArgs(VM_CEIL, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_FMOD:
	    context->callArgs(VM_FMOD, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Double " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_LDEXP:
	    intArg(context, context->sp);
	    floatArg(context, context->nextSp(context->sp));
	    context->callArgs(VM_LDEXP, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Int " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_EXP:
	    context->callArgs(VM_EXP, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_LOG:
	    context->callArgs(VM_LOG, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_LOG10:
	    context->callArgs(VM_LOG10, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_POW:
	    context->callArgs(VM_POW, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Double " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_SQRT:
	    context->callArgs(VM_SQRT, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_COS:
	    context->callArgs(VM_COS, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_SIN:
	    context->callArgs(VM_SIN, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_TAN:
	    context->callArgs(VM_TAN, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_ACOS:
	    context->callArgs(VM_ACOS, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_ASIN:
	    context->callArgs(VM_ASIN, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_ATAN:
	    context->callArgs(VM_ATAN, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_ATAN2:
	    context->callArgs(VM_ATAN2, tmpRef(sp));
	    fprintf(context->stream, Double " %s, " Double " %s)\n",
		    tmpRef(context->nextSp(context->sp)),
		    tmpRef(context->sp));
	    result(context);
	    return;

	case KF_COSH:
	    context->callArgs(VM_COSH, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_SINH:
	    context->callArgs(VM_SINH, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	case KF_TANH:
	    context->callArgs(VM_TANH, tmpRef(sp));
	    fprintf(context->stream, Double " %s)\n", tmpRef(context->sp));
	    result(context);
	    return;

	default:
	    switch (offStack(context, sp)) {
	    case LPC_TYPE_INT:
		context->callArgs(VM_KFUNC_INT, tmpRef(sp));
		fprintf(context->stream, "i16 %u, i32 %u)\n", kfun.func,
			kfun.nargs);
		context->sp = sp;
		return;

	    case LPC_TYPE_FLOAT:
		context->callArgs(VM_KFUNC_FLOAT, tmpRef(sp));
		fprintf(context->stream, "i16 %u, i32 %u)\n", kfun.func,
			kfun.nargs);
		context->sp = sp;
		return;
	    }

	    context->voidCallArgs(VM_KFUNC);
	    fprintf(context->stream, "i16 %u, i32 %d)\n", kfun.func,
		    kfun.nargs);
	    break;
	}
	break;

    case KFUNC_LVAL:
	context->voidCallArgs(VM_KFUNC);
	fprintf(context->stream, "i16 %u, i32 %d)\n", kfun.func, kfun.nargs);
	break;

    case KFUNC_SPREAD:
	switch (offStack(context, sp)) {
	case LPC_TYPE_INT:
	    context->callArgs(VM_KFUNC_SPREAD_INT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i32 %u)\n", kfun.func,
		    kfun.nargs);
	    context->sp = sp;
	    return;

	case LPC_TYPE_FLOAT:
	    context->callArgs(VM_KFUNC_SPREAD_FLOAT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i32 %u)\n", kfun.func,
		    kfun.nargs);
	    context->sp = sp;
	    return;
	}

	context->voidCallArgs(VM_KFUNC_SPREAD);
	fprintf(context->stream, "i16 %u, i32 %u)\n", kfun.func, kfun.nargs);
	break;

    case KFUNC_SPREAD_LVAL:
	context->voidCallArgs(VM_KFUNC_SPREAD_LVAL);
	fprintf(context->stream, "i16 %u, i16 %u, i32 %u)\n", kfun.lval,
		kfun.func, kfun.nargs);
	break;

    case DFUNC:
	switch (offStack(context, sp)) {
	case LPC_TYPE_INT:
	    context->callArgs(VM_DFUNC_INT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i8 %u, i32 %u)\n", dfun.inherit,
		    dfun.func, dfun.nargs);
	    context->sp = sp;
	    return;

	case LPC_TYPE_FLOAT:
	    context->callArgs(VM_DFUNC_FLOAT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i8 %u, i32 %u)\n", dfun.inherit,
		    dfun.func, dfun.nargs);
	    context->sp = sp;
	    return;
	}

	context->voidCallArgs(VM_DFUNC);
	fprintf(context->stream, "i16 %u, i8 %u, i32 %u)\n", dfun.inherit,
		dfun.func, dfun.nargs);
	break;

    case DFUNC_SPREAD:
	switch (offStack(context, sp)) {
	case LPC_TYPE_INT:
	    context->callArgs(VM_DFUNC_SPREAD_INT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i8 %u, i32 %u)\n", dfun.inherit,
		    dfun.func, dfun.nargs);
	    context->sp = sp;
	    return;

	case LPC_TYPE_FLOAT:
	    context->callArgs(VM_DFUNC_SPREAD_FLOAT, tmpRef(sp));
	    fprintf(context->stream, "i16 %u, i8 %u, i32 %u)\n", dfun.inherit,
		    dfun.func, dfun.nargs);
	    context->sp = sp;
	    return;
	}

	context->voidCallArgs(VM_DFUNC_SPREAD);
	fprintf(context->stream, "i16 %u, i8 %u, i32 %u)\n", dfun.inherit,
		dfun.func, dfun.nargs);
	break;

    case FUNC:
	context->voidCallArgs(VM_FUNC);
	fprintf(context->stream, "i16 %u, i32 %u)\n", fun.call, fun.nargs);
	break;

    case FUNC_SPREAD:
	context->voidCallArgs(VM_FUNC_SPREAD);
	fprintf(context->stream, "i16 %u, i32 %u)\n", fun.call, fun.nargs);
	break;

    case CATCH:
	if (context->level == 0) {
	    context->saveLocals();
	}
	ref = context->genRef();
	context->call(VM_CATCH, ref);
	ref2 = context->genRef();
	fprintf(context->stream, "\t%s = call i32 @_setjmp(i8* %s)\n", ref2,
		ref);
	ref = context->genRef();
	fprintf(context->stream, "\t%s = icmp ne i32 %s, 0\n", ref, ref2);
	fprintf(context->stream, "\tbr i1 %s, label %%L%04x, label %%%s\n",
		ref, context->next, context->target(context->block->to[1]));
	context->jumpRelay(context->block->to[1]);
	break;

    case CAUGHT:
	context->voidCallArgs(VM_CAUGHT);
	if (pop) {
	    fprintf(context->stream, "i1 false)\n");
	} else {
	    fprintf(context->stream, "i1 true)\n");
	}
	context->sp = sp;
	return;

    case END_CATCH:
	context->voidCallArgs(VM_CATCH_END);
	if (pop) {
	    fprintf(context->stream, "i1 false)\n");
	} else {
	    fprintf(context->stream, "i1 true)\n");
	}
	context->sp = sp;
	return;

    case RLIMITS:
	context->voidCallArgs(VM_RLIMITS);
	fprintf(context->stream, "i1 true)\n");
	break;

    case RLIMITS_CHECK:
	context->voidCallArgs(VM_RLIMITS);
	fprintf(context->stream, "i1 false)\n");
	break;

    case END_RLIMITS:
	context->voidCall(VM_RLIMITS_END);
	break;

    case RETURN:
	fprintf(context->stream, "\tret void\n");
	break;

    default:
	break;
    }

    if (pop) {
	context->voidCall(VM_POP);
    }
    context->sp = sp;
}

/*
 * emit table for range switch
 */
void ClangCode::emitRangeTable(GenContext *context)
{
    int i;

    fprintf(context->stream,
	    "\n@func%d.%04x = internal constant [%d x " Int "] [" Int " %lld, "
	    Int " %lld",
	    context->num, addr, 2 * (size - 1), (long long) caseRange[1].from,
	    (long long) caseRange[1].to);
    for (i = 2; i < size; i++) {
	fprintf(context->stream, ", " Int " %lld, " Int " %lld",
		(long long) caseRange[i].from, (long long) caseRange[i].to);
    }
    fprintf(context->stream, "], align %d\n", INT_SIZE);
}

/*
 * emit table for string switch
 */
void ClangCode::emitStringTable(GenContext *context)
{
    int i;

    fprintf(context->stream,
	    "\n@func%d.%04x = internal constant [%d x i16] [i16 %u, i16 %u",
	    context->num, addr, 2 * (size - 1), caseString[1].str.inherit,
	    caseString[1].str.index);
    for (i = 2; i < size; i++) {
	fprintf(context->stream, ", i16 %u, i16 %u", caseString[i].str.inherit,
		caseString[i].str.index);
    }
    fprintf(context->stream, "], align 4\n");
}

/*
 * create a Clang code
 */
Code *ClangCode::create(CodeFunction *function)
{
    return new ClangCode(function);
}


ClangBlock::ClangBlock(Code *first, Code *last, CodeSize size) :
    FlowBlock(first, last, size)
{
}

ClangBlock::~ClangBlock()
{
}

/*
 * parameter merged type
 */
Type ClangBlock::mergedParamType(Block *b, LPCParam param, Type type)
{
    CodeSize i;

    if (!b->paramMerged(param)) {
	return LPC_TYPE_VOID;
    }

    i = 0;
    if (type == LPC_TYPE_VOID) {
	for (; i < b->nFrom; i++) {
	    if (b->from[i]->paramOut(param) != 0) {
		type = b->from[i]->paramType(param);
		break;
	    }
	}
    }
    for (; i < b->nFrom; i++) {
	if (b->from[i]->paramOut(param) != 0 &&
	    b->from[i]->paramType(param) != type) {
	    type = LPC_TYPE_VOID;
	    break;
	}
    }
    return type;
}

/*
 * local variable merged type
 */
Type ClangBlock::mergedLocalType(Block *b, LPCLocal local)
{
    Type type;
    CodeSize i;

    if (!b->localMerged(local)) {
	return LPC_TYPE_VOID;
    }

    type = LPC_TYPE_MIXED;
    for (i = 0; i < b->nFrom; i++) {
	if (b->from[i]->localOut(local) != 0) {
	    type = b->from[i]->localType(local);
	    break;
	}
    }
    for (; i < b->nFrom; i++) {
	if (b->from[i]->localOut(local) != 0 &&
	    b->from[i]->localType(local) != type) {
	    type = LPC_TYPE_MIXED;
	    break;
	}
    }
    return type;
}

/*
 * emit instructions for all blocks
 */
void ClangBlock::emit(GenContext *context, CodeFunction *function)
{
    Block *b;
    LPCParam nParams, n;
    CodeSize i;
    Type type;
    int ref;
    bool phi;
    StackSize sp;
    Code *code;

    FlowBlock::evaluate(context);

    fprintf(context->stream, "Lparam:\n");
    nParams = function->nargs + function->vargs;
    for (n = 1; n <= nParams; n++) {
	switch (function->proto[n].type) {
	case LPC_TYPE_INT:
	    context->callArgs(VM_PARAM_INT,
			      ClangCode::paramRef(nParams - n, 0));
	    fprintf(context->stream, "i8 %d)\n", nParams - n);
	    break;

	case LPC_TYPE_FLOAT:
	    context->callArgs(VM_PARAM_FLOAT,
			      ClangCode::paramRef(nParams - n, 0));
	    fprintf(context->stream, "i8 %d)\n", nParams - n);
	    break;

	default:
	    continue;
	}
    }
    fprintf(context->stream, "\tbr label %%L0000\n");

    for (b = this; b != NULL; b = b->next) {
	if (b->nFrom == 0 && b != this) {
	    continue;
	}
	fprintf(context->stream, "L%04x:\n", b->first->addr);
	context->prepareGen(b);

	if (b->nFrom > 1) {
	    /*
	     * stack
	     */
	    for (n = 0, sp = b->sp; sp != STACK_EMPTY;
		 n++, sp = context->nextSp(sp)) {
		if (ClangCode::offStack(context, sp) == LPC_TYPE_NIL) {
		    continue;
		}
		switch (context->get(sp).type) {
		case LPC_TYPE_INT:
		    fprintf(context->stream, "\t%s = phi " Int,
			    ClangCode::tmpRef(sp));
		    break;

		case LPC_TYPE_FLOAT:
		    fprintf(context->stream, "\t%s = phi " Double,
			    ClangCode::tmpRef(sp));
		    break;

		default:
		    continue;
		}
		phi = true;

		for (i = 0; i < b->nFrom; i++) {
		    if (!phi) {
			fprintf(context->stream, ",");
		    }
		    phi = false;
		    fprintf(context->stream, " [%s, %%%s]",
			    ClangCode::tmpRef(context->nextSp(b->from[i]->endSp,
							      n)),
			    GenContext::label(b->from[i], b));
		}
		fprintf(context->stream, "\n");
	    }

	    /*
	     * locals
	     */
	    for (n = 0; n < context->nLocals; n++) {
		ref = b->localIn(n);
		switch (mergedLocalType(b, n)) {
		case LPC_TYPE_INT:
		    fprintf(context->stream, "\t%s = phi " Int,
			    ClangCode::localPhi(n, ref));
		    break;

		case LPC_TYPE_FLOAT:
		    fprintf(context->stream, "\t%s = phi " Double,
			    ClangCode::localPhi(n, ref));
		    break;

		default:
		    continue;
		}

		phi = true;
		for (i = 0; i < b->nFrom; i++) {
		    if (!phi) {
			fprintf(context->stream, ",");
		    }
		    phi = false;
		    fprintf(context->stream, " [%s, %%%s]",
			    ClangCode::localRef(n, b->from[i]->localOut(n)),
			    GenContext::label(b->from[i], b));
		}
		fprintf(context->stream, "\n");
	    }
	}

	if (b->nFrom > (int) (b != this)) {
	    /*
	     * parameters
	     */
	    for (n = 0; n < context->nParams; n++) {
		ref = b->paramIn(n);
		type = (b == this) ?
			function->proto[nParams - n].type : LPC_TYPE_VOID;
		switch (mergedParamType(b, n, type)) {
		case LPC_TYPE_INT:
		    fprintf(context->stream, "\t%s = phi " Int,
			    ClangCode::paramPhi(n, ref));
		    break;

		case LPC_TYPE_FLOAT:
		    fprintf(context->stream, "\t%s = phi " Double,
			    ClangCode::paramPhi(n, ref));
		    break;

		default:
		    continue;
		}

		phi = true;
		if (b == this) {
		    fprintf(context->stream, " [%s, %%Lparam]",
			    ClangCode::paramRef(n, 0));
		    phi = false;
		}
		for (i = 0; i < b->nFrom; i++) {
		    if (!phi) {
			fprintf(context->stream, ",");
		    }
		    phi = false;
		    fprintf(context->stream, " [%s, %%%s]",
			    ClangCode::paramRef(n, b->from[i]->paramOut(n)),
			    GenContext::label(b->from[i], b));
		}
		fprintf(context->stream, "\n");
	    }

	    /*
	     * copy from phi
	     */
	    for (n = 0; n < context->nParams; n++) {
		ref = b->paramIn(n);
		type = (b == this) ?
			function->proto[nParams - n].type : LPC_TYPE_VOID;
		switch (mergedParamType(b, n, type)) {
		case LPC_TYPE_INT:
		    context->copyInt(ClangCode::paramRef(n, ref),
				     ClangCode::paramPhi(n, ref));
		    break;

		case LPC_TYPE_FLOAT:
		    context->copyFloat(ClangCode::paramRef(n, ref),
				       ClangCode::paramPhi(n, ref));
		    break;
		}
	    }
	}

	if (b->nFrom > 1) {
	    /*
	     * copy locals from phi
	     */
	    for (n = 0; n < context->nLocals; n++) {
		ref = b->localIn(n);
		switch (mergedLocalType(b, n)) {
		case LPC_TYPE_INT:
		    context->copyInt(ClangCode::localRef(n, ref),
				     ClangCode::localPhi(n, ref));
		    break;

		case LPC_TYPE_FLOAT:
		    context->copyFloat(ClangCode::localRef(n, ref),
				       ClangCode::localPhi(n, ref));
		    break;
		}
	    }
	}

	context->sp = b->sp;
	context->level = b->level;
	context->line = b->first->line;

	/*
	 * update line number at the start of the block
	 */
	if (b->nFrom == 0 && context->line != 0) {
	    context->voidCallArgs(VM_LINE);
	    fprintf(context->stream, "i16 %u)\n", context->line);
	} else {
	    for (i = 0; i < b->nFrom; i++) {
		if (context->line != b->from[i]->last->line) {
		    context->voidCallArgs(VM_LINE);
		    fprintf(context->stream, "i16 %u)\n", context->line);
		    break;
		}
	    }
	}

	/*
	 * emit code for the block
	 */
	for (code = b->first; ; code = code->next) {
	    if (code == b->last && context->level == 0) {
		switch (code->instruction) {
		case Code::JUMP_ZERO:
		case Code::JUMP_NONZERO:
		case Code::SWITCH_INT:
		case Code::SWITCH_RANGE:
		case Code::SWITCH_STRING:
		case Code::CATCH:
		    context->saveBeforeMerge(b);
		    break;

		default:
		    break;
		}
	    }
	    code->emit(context);
	    if (code->instruction == Code::END_CATCH) {
		context->level--;
	    }
	    if (code == b->last) {
		switch (code->instruction) {
		case Code::JUMP_ZERO:
		case Code::JUMP_NONZERO:
		case Code::SWITCH_INT:
		case Code::SWITCH_RANGE:
		case Code::SWITCH_STRING:
		case Code::CATCH:
		case Code::RETURN:
		    break;

		default:
		    if (context->level == 0) {
			context->saveBeforeMerge(b);
		    }
		    fprintf(context->stream, "\tbr label %%%s\n",
			    context->target(b->to[0]));
		    context->jumpRelay(b->to[0]);
		    break;
		}
		break;
	    }
	}
    }
}

/*
 * create a Clang block
 */
Block *ClangBlock::create(Code *first, Code *last, CodeSize size)
{
    return new ClangBlock(first, last, size);
}


ClangObject::ClangObject(CodeObject *object, CodeByte *prog, int nFunctions)
{
    this->object = object;
    this->prog = prog;
    this->nFunctions = nFunctions;
}

ClangObject::~ClangObject()
{
}

/*
 * generate jit header
 */
void ClangObject::header(FILE *stream)
{
    fprintf(stream, "target triple = \"%s\"\n\n", TARGET_TRIPLE);
    fprintf(stream, "declare i32 @_setjmp(i8*) #0\n");
}

/*
 * generate jit function table
 */
void ClangObject::table(FILE *stream, int nFunctions)
{
    int i;

    fprintf(stream, "@functions ="
# ifdef WIN32
				 " dllexport"
# endif
					    " constant [%d x void (i8**, i8*)*] [",
	    nFunctions + 1);
    for (i = 1; i <= nFunctions; i++) {
	fprintf(stream, "void (i8**, i8*)* @func%d, ", i);
    }
    fprintf(stream, "void (i8**, i8*)* null], align %d\n",
	    (int) sizeof(void *));
}

/*
 * create a dynamically loadable object
 */
bool ClangObject::emit(char *base)
{
    char buffer[1000];
    FILE *stream;
    int i;

    /*
     * generate .ll file
     */
    sprintf(buffer, "%s.ll", base);
    stream = fopen(buffer, "w");

    header(stream);

    table(stream, nFunctions);

    for (i = 1; i <= nFunctions; i++) {
	CodeFunction func(object, prog);
	Block *b = Block::function(&func);

	fprintf(stream,
		"\ndefine internal void @func%d(i8** %%vmtab, i8* %%f) #1 {\n",
		i);
	if (b != NULL) {
	    GenContext context(stream, &func, b->fragment(), i);
	    ClangCode *code;

	    b->emit(&context, &func);
	    fprintf(stream, "}\n");
	    for (code = context.switchList; code != NULL; code = code->list) {
		if (code->instruction == Code::SWITCH_RANGE) {
		    code->emitRangeTable(&context);
		} else {
		    code->emitStringTable(&context);
		}
	    }
	    b->clear();
	} else {
	    fprintf(stream, "L0000:\n\tret void\n}\n");
	}
	prog = func.endProg();
    }

    /* attributes */
    fprintf(stream, "attributes #0 = { nounwind returns_twice }\n");
    if (nFunctions != 0) {
	fprintf(stream, "attributes #1 = { nounwind "
			"\"no-frame-pointer-elim\"=\"false\" }\n");
    }

    fclose(stream);

    /*
     * compile .ll file to shared object
     */
    sprintf(buffer,
# ifndef WIN32
	    "clang -fPIC"
# else
	    "\"\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\Llvm\\bin\\clang.exe\"\""
# endif
# ifndef LLVM3_6
	    " -march=native"
# endif
	    " -Os -shared"
# if defined(__APPLE__) || defined(WIN32)
	    " -Wno-override-module"
# endif
# ifdef __APPLE__
	    " -undefined dynamic_lookup"
# endif
# ifndef WIN32
	    " -o %s.so"
# else
	    " -o %s.dll"
# endif
	    " %s.ll", base, base);
    return (system(buffer) == 0);
}
