class ClangCode : public FlowCode {
public:
    ClangCode(CodeFunction *function);
    virtual ~ClangCode();

    virtual void emit(class GenContext *context);
    void emitRangeTable(class GenContext *context);
    void emitStringTable(class GenContext *context);

    static bool onStack(class GenContext *context, StackSize sp);
    static char *tmpRef(StackSize sp);
    static char *paramRef(LPCParam param, int ref);
    static char *paramPhi(LPCParam param, int ref);
    static char *localRef(LPCLocal local, int ref);
    static char *localPhi(LPCLocal local, int ref);
    static Code *create(CodeFunction *function);

    ClangCode *list;	/* next in switch list */

private:
    char *paramRef(class GenContext *context, LPCParam param);
    char *localRef(class GenContext *context, LPCLocal local);
    void result(class GenContext *context);
    void switchInt(GenContext *context, CodeSize defAddr);
    void genTable(class GenContext *context, const char *type);
};

class ClangBlock : public FlowBlock {
public:
    ClangBlock(Code *fist, Code *last, CodeSize size);
    virtual ~ClangBlock();

    virtual void emit(class GenContext *context, CodeFunction *function);

    static Block *create(Code *first, Code *last, CodeSize size);
};

class ClangObject {
public:
    ClangObject(CodeObject *object, uint8_t *prog, int nFunctions);
    virtual ~ClangObject();

    void emit(char *base);

    static bool init(const char *base);

private:
    void header(FILE *stream);
    void table(FILE *stream, int nFunctions);

    CodeObject *object;
    uint8_t *prog;
    int nFunctions;
};
