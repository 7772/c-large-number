#include <stdio.h>
#include <stdlib.h>

#define	MAXDIGITS	100		/* maximum length bignum */ 

#define PLUS		1		/* positive sign bit */
#define MINUS		-1		/* negative sign bit */

typedef struct {
    char digits[MAXDIGITS]; /* represent the number */
	int signbit;			/* 1 if positive, -1 if negative */ 
    int lastdigit;			/* index of high-order digit */
} bignum;

int subtract_bignum(bignum *a, bignum *b, bignum *c);
void zero_justify(bignum *n);
int compare_bignum(bignum *a, bignum *b);
// void print_bignum(bignum *n);
// void int_to_bignum(int s, bignum *n);
// void initialize_bignum(bignum *n);
// int max(int a, int b);
// void add_bignum(bignum *a, bignum *b, bignum *c);
// void subtract_bignum(bignum *a, bignum *b, bignum *c);

void print_bignum(bignum *n) {
	int i;

	if (n->signbit == MINUS) printf("- ");
	for (i=n->lastdigit; i>=0; i--) {
		printf("%c",'0'+ n->digits[i]);
        // printf("%c",n->digits[i]);
    }

	printf("\n");
}

// int 형의 입력값을 bignum 구조체의 digits 캐릭터 배열에 담는 함수
void int_to_bignum(int s, bignum *n) {
	int i;				/* counter */
	int t;				/* int to work with */

    // 입력된 수의 부호값 지정
	if (s >= 0) n->signbit = PLUS;
	else n->signbit = MINUS;

    // 배열 초기화
	for (i = 0; i < MAXDIGITS; i++) n->digits[i] = (char) 0;

	n->lastdigit = -1;

    // 절대값 구하기
	t = abs(s);

	while (t > 0) {
		n->lastdigit ++;
        // 나머지가 차례로 들어가면서 배열에 각각의 자리수를 저장
		n->digits[ n->lastdigit ] = (t % 10);
		t = t / 10;
	}

	if (s == 0) n->lastdigit = 0;
}

void initialize_bignum(bignum *n)
{
	int_to_bignum(0, n);
}

int max(int a, int b)
{
	if (a > b) return(a); else return(b);
}

/*	c = a (+,-,/,*) b;	*/

void add_bignum(bignum *a, bignum *b, bignum *c)
{
	int carry;			/* carry digit */
	int i;				/* counter */

	initialize_bignum(c);

    // 같은 부호의 수 이면
    // 결과값의 부호값도 동일하게 설정
	if (a->signbit == b->signbit) c->signbit = a->signbit;
    // 다른 부호의 수 이면
	else {
		if (a->signbit == MINUS) {
			a->signbit = PLUS;
			subtract_bignum(b,a,c);
			a->signbit = MINUS;
		} else {
            b->signbit = PLUS;
            subtract_bignum(a,b,c);
            b->signbit = MINUS;
		}
		return;
	}

    // 큰 자리수의 값을 구한 다음
	c->lastdigit = max(a->lastdigit,b->lastdigit)+1;
	carry = 0;

    // 그 값만큼 더해줌
	for (i = 0; i <= (c->lastdigit); i++) {
		c->digits[i] = (char) (carry + a->digits[i] + b->digits[i]) % 10;
		carry = (carry + a->digits[i] + b->digits[i]) / 10;
	}

	zero_justify(c);
}

int subtract_bignum(bignum *a, bignum *b, bignum *c)
{
	int borrow;			/* has anything been borrowed? */
	int v;				/* placeholder digit */
	int i;				/* counter */

	initialize_bignum(c);

    // 둘 중 하나라도 음수이면
    // c 에는 두 수를 합친 값이 저장되고 함수 종료
	if ((a->signbit == MINUS) || (b->signbit == MINUS)) {
        b->signbit = -1 * b->signbit;
        add_bignum(a,b,c);
        b->signbit = -1 * b->signbit;
		return -1;
    }

    // 두 수를 비교하여 두번째 값이 더 크다면 
    // 두번째값에서 첫번째 값을 빼주도록 재귀호출한다.
	if (compare_bignum(a,b) == PLUS) {
		subtract_bignum(b,a,c);
		c->signbit = MINUS;
		return -1;
	}

    // 더 큰 자리수를 구한다.
    c->lastdigit = max(a->lastdigit,b->lastdigit);
    borrow = 0;

    // 큰 자리수 만큼 반복문을 수행하면서
    for (i = 0; i <= (c->lastdigit); i++) {
        // 각 배열의 첫번째값과 빌린 수를 표현하는 borrow 을 뺀 후 
        // 임시값 v 에 담아둔다.
        v = (a->digits[i] - borrow - b->digits[i]);

        // 만일 첫째값의 첫번째 자리수가 0보다 크면 borrow 는 0 이다.
        if (a->digits[i] > 0)
            borrow = 0;
        // 만일 v 값이 0 보다 작으면 v = v + 10 을 수행하고 
        // borrow 를 1로 저장한다.
        if (v < 0) {
            v = v + 10;
            borrow = 1;
        }
        // 임시 값 v 를 10으로 나눈 나머지를 결과값에 할당한다.
        c->digits[i] = (char) v % 10;
    }

	zero_justify(c);

    return 0;
}

int compare_bignum(bignum *a, bignum *b)
{
	int i;				/* counter */

	if ((a->signbit == MINUS) && (b->signbit == PLUS)) return(PLUS);
	if ((a->signbit == PLUS) && (b->signbit == MINUS)) return(MINUS);

	if (b->lastdigit > a->lastdigit) return (PLUS * a->signbit);
	if (a->lastdigit > b->lastdigit) return (MINUS * a->signbit);

	for (i = a->lastdigit; i>=0; i--) {
		if (a->digits[i] > b->digits[i]) return(MINUS * a->signbit);
		if (b->digits[i] > a->digits[i]) return(PLUS * a->signbit);
	}

	return(0);
}

void zero_justify(bignum *n)
{
	while ((n->lastdigit > 0) && (n->digits[ n->lastdigit ] == 0))
		n->lastdigit --;

        if ((n->lastdigit == 0) && (n->digits[0] == 0))
		n->signbit = PLUS;	/* hack to avoid -0 */
}

void digit_shift(bignum *n, int d)		/* multiply n by 10^d */
{
	int i;				/* counter */

	if ((n->lastdigit == 0) && (n->digits[0] == 0)) return;

	for (i = n->lastdigit; i >= 0; i--)
		n->digits[i+d] = n->digits[i];

	for (i = 0; i < d; i++) n->digits[i] = 0;

	n->lastdigit = n->lastdigit + d;
}

void multiply_bignum(bignum *a, bignum *b, bignum *c) {
	bignum row;			/* represent shifted row */
	bignum tmp;			/* placeholder bignum */
	int i,j;			/* counters */

	initialize_bignum(c);

    // row 에 첫번째 값을 나타내는 구조체변수를 할당한다.
	row = *a;

    // a 를 b의 자리수 만큼 더한다.

    // 첫번째 반복문은 두번째 값의 자리 수 만큼 반복한다.
	for (i = 0; i <= b->lastdigit; i++) {
        // 두번째 반복문은 두번째 자리의 각 자리의 수(index) 만큼 반복한다.
		for (j = 1; j <= b->digits[i]; j++) {
            // 첫번째 수를 b의 각 자리수 만큼 더해준다.
			add_bignum(c,&row,&tmp);
			*c = tmp;
		}
        // row 를 한 칸 shift 해준다.
		digit_shift(&row,1);
	}

	c->signbit = a->signbit * b->signbit;

	zero_justify(c);
}

void divide_bignum(bignum *a, bignum *b, bignum *c) {
    bignum row;             /* represent shifted row */
    bignum tmp;             /* placeholder bignum */
	int asign, bsign;		/* temporary signs */
    int i,j;                /* counters */

	initialize_bignum(c);

	c->signbit = a->signbit * b->signbit;

	asign = a->signbit;
	bsign = b->signbit;

	a->signbit = PLUS;
    b->signbit = PLUS;

	initialize_bignum(&row);
	initialize_bignum(&tmp);

	c->lastdigit = a->lastdigit;

    // 첫번째 변수의 마지막 자리수부터 0까지 반복한다.
	for (i = a->lastdigit; i >= 0; i--) {
		digit_shift(&row,1);
		row.digits[0] = a->digits[i];
		c->digits[i] = 0;

        // 비교하는 두 값중에 두번째 값이 더 크다면 반복해서 현재 row 에서 b를 빼준다.
		while (compare_bignum(&row,b) != PLUS) {
			c->digits[i] ++;
			subtract_bignum(&row,b,&tmp);
            // 뺀 값을 다시 row 로 할당한다.
			row = tmp;
		}
	}

	zero_justify(c);

	a->signbit = asign;
	b->signbit = bsign;
}

int main() {
	int a,b;
	bignum n1, n2, n3, zero;

	while (scanf("%d %d\n",&a,&b) != EOF) {
		printf("a = %d    b = %d\n",a,b);
		int_to_bignum(a,&n1);
		int_to_bignum(b,&n2);

		add_bignum(&n1,&n2,&n3);
		printf("addition : ");
		print_bignum(&n3);

		printf("compare_bignum a ? b = %d\n", compare_bignum(&n1, &n2));

		subtract_bignum(&n1,&n2,&n3);
		printf("subtraction : ");
		print_bignum(&n3);

        multiply_bignum(&n1,&n2,&n3);
		printf("multiplication : ");
        print_bignum(&n3);

		int_to_bignum(0,&zero);

		if (compare_bignum(&zero, &n2) == 0) {
			printf("division : NaN \n");
        }
        else {
			divide_bignum(&n1,&n2,&n3);
			printf("division : ");
            print_bignum(&n3);
		}
		printf("--------------------------\n");
	}

    return 0;
}
