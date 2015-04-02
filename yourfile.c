#include <stdio.h>
#include <stdlib.h>

const int MAXN = 256;
const int NO_VARS = 3;
int no_edges;

int edges[256][2];
int size;


int vartonum(char x)
{
	if(x=='x') return 0;
	if(x=='y') return 1;
	if(x=='z') return 2;
	return -1;
}

int varchar(char x)
{
	if(vartonum(x)<0) return -1;
	return 1;
}

int quantchar(char x)
{
	if(x=='A') return 0;
	if(x=='E') return 1;
	return -1;
}

int bc(char x)
{
	if(x=='^') return 0;
	if(x=='v') return 1;
	if(x=='>') return 2;
	return -1;
}

int length(char *str)
{
	int l = 0;
	while (str[l] != '\0') l++;
	return l;
}
	
int is_bin(char *g)
{
	////printf("GOING THROUGH %s\n", g);
	if(g[0] != '(' || g[length(g)-1] != ')' || length(g) == 0) 
	{
		return -1;
		
	}
	int ans = 0;
	int i = 1;
	int var = 1;
	
	while(var)
	{
		var = 0;
		if(g[i] == '(')
		{
			int cnt = 1;
			i++;
			while(cnt)
			{
				
				if(g[i] == '(') cnt++;
				if(g[i] == ')') cnt--;
				i++;
				
			}
			var = 0;
		}
		if((g[i] == 'A' || g[i] == 'E') && varchar(g[i+1])>=0 && var==0)
		{
			i += 2;
			var = 1;
		}
		if(g[i] == 'X' && g[i+1] == '[' && g[i+4] == ']' && varchar(g[i+2])>=0 && varchar(g[i+3])>=0 && var==0)
		{
			i += 5;
			var = 0;
		}
	}
	////printf("CONN %c for g = %s where i = %d\n",g[i], g, i);
	if(bc(g[i]) >= 0)
	{
		ans = i;
	}
	else
	{
		return -1;
	}
	
	return ans;
}

char* segment(char *g, int b, int e)
{
	char* a = malloc((e-b+1) * sizeof(char));;
	if(e - b<=0) return malloc((1) * sizeof(char));
	int i;
	for(i=b; i<e; i++)
	{
		a[i-b] = g[i];
	}
	return a;
}

int are_connected(int x, int y)
{
	int i;
	for(i=0;i<no_edges; i++)
		if(edges[i][0] == x && edges[i][1] == y) return 1;
	return 0;
}

int X_eval(int x, int y, int V[3])
{
	if(are_connected(V[x], V[y])) return 1;
	return 0;
}


/////////////////////BIN////////////////////
int negation(int a)
{
	if(a==1) return 0;
	return 1;
}

int bin_eval(int a, int b, int cn)
{
	switch(cn)
	{
		case 0: if(a==1 && b==1) return 1;
			return 0;
		case 1: if(a==1 || b==1) return 1;
			return 0;
		case 2: if(a==0 || (a==1 && b==1)) return 1;
			return 0;
	}
	return -1;
}
/////////////////////BIN////////////////////


int go(char *g, int V[3])
{
	int ev, i;
	//printf("GOING THROUGH %s\n", g);
	if(g[0] == '-') 
	{
		ev =  negation(go(segment(g, 1, length(g)), V));
		//printf("%s is negated, the eval is: ", g);
	}
	if(g[0] == 'A')
	{
		ev = 1;
		for(i=0; i<size; i++)
		{
			V[vartonum(g[1])] = i;
			if(go(segment(g, 2, length(g)), V) == 0) ev = 0;
		}
		//printf("%s is universal, the eval is: ", g);
	}
	if(g[0] == 'E')
	{
		ev = 0;
		for(i=0; i<size; i++)
		{
			V[vartonum(g[1])] = i;
			if(go(segment(g, 2, length(g)), V) == 1) ev = 1;
		}
		//printf("%s is existential, the eval is: ", g);
	}
	if(g[0] == 'X')
	{
		ev = X_eval(vartonum(g[2]), vartonum(g[3]), V);
		//printf("%s is simple, for assigment x: %d y: %d z: %d the eval is: ", g, V[0], V[1], V[2]);
	}
	if(is_bin(g)>=0)
	{
		ev =  bin_eval(go(segment(g, 1, is_bin(g)), V), go(segment(g, is_bin(g)+1, length(g)-1), V), bc(g[is_bin(g)]));
		//printf("%s is binary, the eval is: ", g);
	}
	//printf("%d\n", ev);
	return ev;
}

int eval(char *fmla, int _edges[no_edges][2], int _size, int _V[3])
{
	int V[3];
	int i;
	for(i=0; i<no_edges; i++)
	{
		edges[i][0] = _edges[i][0];
		edges[i][1] = _edges[i][1];
	}
	for(i=0; i<NO_VARS; i++)
	{
		V[i] = _V[i];
	}
	size = _size;
	return go(fmla, V);
}



int parse(char *g)
{
	if(length(g) < 5) return 0;
	if(g[0] == '-') return 2*((_Bool) parse(segment(g, 1, length(g))));
	if(g[0] == 'A') return 5*((_Bool) parse(segment(g, 2, length(g))));
	if(g[0] == 'E') return 4*((_Bool) parse(segment(g, 2, length(g))));
	if(g[0] == 'X' && g[1] == '[' && g[4] == ']' && varchar(g[2])>=0 && varchar(g[3])>=0 && length(g) == 5) return 1;
	int k = is_bin(g);
	////printf("IS_BIN %d for g = %s\n", k, g);
	if(k>=0) return 3*((_Bool) parse(segment(g, 1, k)) * (_Bool) parse(segment(g, k+1, length(g)-1)));
	
	return 0;
}

// int main(void)
// {
// 	char g[256];
// 	//printf("Enter formula\n");
// 	scanf("%s", g);
// 	//printf("%d - ", parse(g));
// 	
// 	int p=parse(g);
// 	switch(p)
// 	{case 0: //printf("Not a formula");break;
// 	case 1: //printf("An atomic formula");break;
// 	case 2: //printf("A negated formula");break;
// 	case 3: //printf("A binary connective formula");break;
// 	case 4: //printf("An existential formula");break;
// 	case 5: //printf("A universal formula");break;
// 	default: //printf("Not a formula");break;
// 	}
// 	if(parse(g) == 0) return 0;
// 	//printf("\n Enter size of graph: ");
// 	scanf("%d", &size);
// 	int i, AV[NO_VARS];
// 	for(i=0; i<no_edges; i++)
// 	{
// 		//printf("\n Enter %d-th edge (two connected verticles): ", i+1);
// 		scanf("%d%d", &edges[i][0], &edges[i][1]);
// 	}
// 	//printf("\n Enter values of x, y and z: ");
// 	for(i=0; i<NO_VARS; i++)
// 	{
// 		
// 		scanf("%d", &AV[i]);
// 	}
// 	//printf("The result is: ");
// 	//printf("%d", eval(g, edges, size, AV));
// 	return 0;
// }