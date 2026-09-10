#define CHOICE(e,f,g) (((e)&(f))|((~(e))&(g)))
#define MEDIAN(e,f,g) (((e)&(f))|((f)&(g))|((g)&(e)))
#define ROTATE(a,n) (((a)>>(n))|((a)<<(32-(n))))
