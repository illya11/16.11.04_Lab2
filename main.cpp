#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include "time.h"

using namespace std;

int pow (int x, int n) // return x^n
{
    int res = 1;
    for (int i=0; i<n; i++) res*=x;
    return res;
}

int log10 (int x)
{
    int res=0;
    while (x>1)
    {
        res++;
        x/=10;
    }
    return res;
}

string int_to_str(int number, int len)
{
    string s;
    if (number == 0)
    {
        s="0";
        for (int i=1; i<len; i++) s+="0";
        return s;
    }
    while (number>0)
    {
        s = char(number%10 + 48) + s;
        number/=10;
    }
    for (int i=s.length(); i<len; i++) s="0"+s;
    return s;
}

/******************************************************
*******************************************************
******************* LONG NUMBER ***********************
*******************************************************
*******************************************************
*******************************************************/

class LNum
{
public:
    bool sign = 1; // 1<=> +  0<=> -
    vector <int> num;
    static const int base = 10000; // should be 10^N (perfectly <= 10^9)
public:
    LNum ()
    {
        LNum(0);
    }

    LNum (const vector <int> &a)
    {
        num = a;
    }

    LNum (const vector <int> &a, bool s)
    {
        num = a;
        sign = s;
    }

    LNum (const string &ss)   // 10
    {
        string s=ss;
        if (s[0]=='-')
        {
            sign = 0;
            s.erase(s.begin(),s.begin()+1);
        }
        vector <int> cur;
        //cout << "FOR " << ss << "\ns.length() = " << s.length() << " log10(base) = " << log10(base) << endl;
        if (s.length()%log10(base)>0)// adding first number
        {
            string cur_number = s.substr(0, s.length()%log10(base));
            cur.push_back(atoi(cur_number.c_str()));
        }
        for (int i=s.length()%log10(base); i<s.length(); i+=log10(base))
        {
            string cur_number = s.substr(i, log10(base));
            cur.push_back(atoi(cur_number.c_str()));
        }
        num = cur;
    }

    LNum (int a) // 10
    {
        if (a<0) {sign = 0; a=-a;}
        vector <int> cur;
        while (a>0)
        {
            cur.push_back(a%base);
            a=a/base;
        }
        reverse(cur.begin(), cur.end());
        num = cur;
    }

    void operator = (const string &s)
    {
        LNum cur (s);
        sign = cur.sign;
        num = cur.num;
    }

    int size ()
    {
        return this->num.size();
    }

    int geti (int i)
    {
        if (this->size() <= i) return 0;
        return this->num[i];
    }

    void cut0 ()
    {
        int i=0;
        while (i<this->size() && this->num[i]==0) i++;
        if (i>0 && i==this->size()) i--;
        this->num.erase(this->num.begin(), this->num.begin()+i);
    }

    friend bool operator == (LNum& left, LNum& right);
    friend bool operator == (LNum& left, int& right);
    friend bool operator < (LNum& left, LNum& right);
    friend bool operator > (LNum& left, LNum& right);
    friend bool operator >= (LNum& left, LNum& right);
    friend bool operator <= (LNum& left, LNum& right);

    friend const LNum operator+(LNum& left, LNum& right);
    friend const LNum operator-(LNum& left, LNum& right);
    friend LNum& operator+=(LNum& left, LNum& right);
    friend const LNum operator%(LNum left, LNum right);
    friend ostream& operator<<(ostream& os, const LNum &a);
    friend istream& operator>>(istream& is, LNum &a);

    operator int ()
    {
        if (this->size()==0) return 0;
        this->cut0();
        string s = "";
        if (this->sign==0) s = "-";
        vector <int> p = this->num;
        for (vector <int>::iterator it=p.begin(); it!=p.end(); it++)
            if (it!=p.begin()) s+=int_to_str(*it, log10(this->base)); else s+=int_to_str(*it,0);
        cout << "int = " << s << endl;
        return atoi(s.c_str());
    }

    operator double ()
    {
        if (this->size()==0) return 0;
        this->cut0();
        string s = "";
        if (this->sign==0) s = "-";
        vector <int> p = this->num;
        for (vector <int>::iterator it=p.begin(); it!=p.end(); it++)
            if (it!=p.begin()) s+=int_to_str(*it, log10(this->base)); else s+=int_to_str(*it,0);
        return atof(s.c_str());
    }

    // Karatsuba Method ******************************************************************************************************
    void pow10 (int n)
    {
        if (this->size()==1 && this->get0()==0) return;
        for (int i=0; i<n; i++)
        {
            this->num.push_back(0);
        }
    }
    int get0 ()
    {
        return geti(0);
    }

    LNum L (int n)
    {
        vector <int> rez;
            if ((this->size()==1 && this->get0()==0) || (this->size()-n/2 <= 0))
            {
                rez.push_back(0);
                LNum t (rez);
                return t;
            }
        for (int i=0; i<this->size()-n/2; i++)
        {
            rez.push_back(this->num[i]);
        }
        LNum t (rez);
        t.cut0();
        return t;
    }

    LNum R (int n)
    {
        vector <int> rez;
            if (this->size()==1 && this->get0()==0)
            {
                rez.push_back(0);
                LNum t (rez);
                return t;
            }
        for (int i=max(this->size()-n/2, 0); i<this->size(); i++)
        {
            rez.push_back(this->num[i]);
        }
        LNum t (rez);
        t.cut0();
        return t;
    }

    const LNum Karatsuba (LNum& x, LNum& y)
    {
        LNum a = x;
        LNum b = y;
        LNum rez (0);
        bool s = (a.sign + b.sign + 1)%2;
        a.sign = 1;
        b.sign = 1;
        rez = unsigned_Karatsuba(a, b);
        rez.sign = s;
        return rez;
    }

    const LNum unsigned_Karatsuba (LNum& x, LNum& y)  // can make it void function out of class
    {
            //cout << "\tLooking now for  " << x << "  ,  " << y << endl;
        int n = max(x.size(),y.size());
        if (n==1)
        {
            vector <int> rez;
            if (x.get0()==0 || y.get0()==0) return LNum("0");
            rez.push_back((x.get0()*y.get0())%base);
            if ((x.get0()*y.get0())/10>0) rez.push_back((x.get0()*y.get0())/base);
            reverse(rez.begin(),rez.end());
            LNum out (rez);
            //cout << "Case n = 1:   rez = " << out << endl;
            return out;
        }
        n+=n%2;
        LNum YR=y.R(n);
        LNum YL=y.L(n);
        LNum XR=x.R(n);
        LNum XL=x.L(n);
        //cout << "General case:   XL = " << XL << "  XR = " << XR << "\n                YL = " << YL << "  YR = " << YR << endl;
        LNum Prod1 (0);
        LNum Prod2 (0);
        LNum Prod3 (0);
        Prod1 = unsigned_Karatsuba (XL, YL);
        Prod2 = unsigned_Karatsuba (XR, YR);
        LNum temp1 = XL;
        temp1 += XR;
        LNum temp2 = YL;
        temp2 += YR;
        Prod3 = unsigned_Karatsuba (temp1, temp2);
        //cout << "Prod 3 from " << temp1 << " " << temp2 << endl;

        LNum Prod4 ("0");
            //cout << "Prod 1 = " << Prod1 << endl;
            //cout << "Prod 2 = " << Prod2 << endl;
            //cout << "Prod 3 = " << Prod3 << endl;
            //cout << "Prod 4 = " << Prod4 << endl;
        Prod4 = Prod3 - Prod1;
            //cout << "Prod 3 - Prod 1 = " << Prod4 << endl;
        Prod4 = Prod4 - Prod2;
            //cout << "Prod 3 - Prod 1 - Prod 2 = " << Prod4 << endl;
        Prod4.pow10(n/2);
            //cout << "(Prod 3 - Prod 1 - Prod 2)*10^(n/2) = " << Prod4 << endl;
        Prod4 += Prod2;
            //cout << "(Prod 3 - Prod 1 - Prod 2)*10^(n/2) + Prod 2 = " << Prod4 << endl;
        Prod1.pow10(n);
            //cout << "(Prod 1)*10^(n) = " << Prod1 << endl;
        Prod1 += Prod4;
            //cout << "(Prod 1)*10^(n) + (Prod 3 - Prod 1 - Prod 2)*10^(n/2) + Prod 2 = " << Prod1 << endl;
        Prod1.cut0();
        return Prod1;
    }

    // The End of Karatsuba Method ###############################################################################################

    // Usual multiple ******************************************************************************************************
    LNum MultNum (int n)  // 10
    {
        vector <int> rez;
        int delta=0;
        for (int i=this->size()-1; i>=0; i--)
        {
            int m=(this->geti(i))*n + delta;
            rez.push_back(m%base);
            delta=m/base;
        }
        if (delta>0) rez.push_back(delta);
        reverse(rez.begin(), rez.end());
        return LNum(rez, this->sign);
    }

    friend const LNum operator*(LNum left, LNum right);
    // The End of Usual multiple ###############################################################################################

    // Usual divide ******************************************************************************************************
    LNum take_first_N(int n)
    {
        bool s=this->sign;
        vector <int> a;
        for (int i=0; i<min(n,this->size()); i++)
        {
            a.push_back(this->geti(i));
        }
        LNum rez(a,s);
        return rez;
    }

    LNum take_after_first_N(int n)
    {
        bool s=this->sign;
        vector <int> a;
        for (int i=n; i<this->size(); i++)
        {
            a.push_back(this->geti(i));
        }
        LNum rez(a,s);
        return rez;
    }

    void add_to_end (LNum right)
    {
        vector <int> result=this->num;
        copy(right.num.begin(), right.num.end(), back_inserter(result));
        this->num = result;
        this->cut0();
        return;
    }

    friend const LNum operator/(LNum left, LNum right);
    // The End of Usual divide ###############################################################################################

};

const LNum operator-(LNum& left, LNum& right) //10
    {
        if (left.sign==1 && right.sign==0)
        {
            LNum a (left.num, 1);
            LNum b (right.num, 1);
            LNum rez = a+b;
            rez.sign=1;
            return rez;
        }
        if (left.sign==0 && right.sign==1)
        {
            LNum a (left.num, 1);
            LNum b (right.num, 1);
            LNum rez = a+b;
            rez.sign = 0;
            return rez;
        }
        int s=1;
        vector <int> first;
        vector <int> second;
        if (left.sign==0 && right.sign==0)
        {
            if (left<right)
            {
                s=0;
                first = left.num;
                second = right.num;
            } else
            {
                first = right.num;
                second = left.num;
            }
        } else // sign1==sign2==1
        {
            if (left<right)
            {
                s=0;
                first = right.num;
                second = left.num;
            } else
            {
                first = left.num;
                second = right.num;
            }
        }
        vector <int> rez;
        reverse(first.begin(), first.end());
        reverse(second.begin(), second.end());
        int n=first.size();
        int m=second.size();
        for (int i=0; i<m; i++)
        {
            //cout << "first[i]  =  " << first[i] << " second[i]  =  " << second[i] << endl;
            //cout << "first[i] - second[i]  =  " << first[i]-second[i] << endl;
            if ((first[i]-second[i]) < 0)
            {
                rez.push_back(left.base+first[i]-second[i]);
                int j=i+1;
                while (first[j]==0)
                {
                    first[j]=left.base-1;
                    j++;
                }
                first[j]--;
            } else rez.push_back(first[i]-second[i]);
        }
        for (int i=m; i<n; i++) rez.push_back(first[i]);
        reverse(rez.begin(),rez.end());
        LNum t(rez, s);
        t.cut0();
        return t;
    }

const LNum operator+(LNum& left, LNum& right) //10
    {
        if (left.sign==1 && right.sign==0)
        {
            LNum a (left.num, 1);
            LNum b (right.num, 1);
            LNum rez = a-b;
            return rez;
        }
        if (left.sign==0 && right.sign==1)
        {
            LNum a (left.num, 1);
            LNum b (right.num, 1);
            LNum rez = b-a;
            return rez;
        };
        int s=1;
        if (left.sign==0) s=0;
        vector <int> first = left.num;
        vector <int> second = right.num;
        reverse(first.begin(), first.end());
        reverse(second.begin(), second.end());
        //for (vector <int>::iterator it = first.begin(); it<first.end(); it++) cout << *it << " ";
        //cout << endl;
        //for (vector <int>::iterator it = second.begin(); it<second.end(); it++) cout << *it << " ";
        //cout << endl;
        vector <int> rez;
        int delt=0;
        int n=first.size();
        int m=second.size();

        for (int i=0; i<min(n,m); i++)
        {
            int a=first[i]+second[i]+delt;
            delt = a/left.base;
            rez.push_back(a%left.base);
            //cout << "a = " << a << " delt = " << delt << " rez.push_back = " << a%left.base << endl;
        }
        for (int i=min(n,m); i<n; i++)
        {
            int a=first[i]+delt;
            delt = a/left.base;
            rez.push_back(a%left.base);
        }
        for (int i=min(n,m); i<m; i++)
        {
            int a=second[i]+delt;
            delt = a/left.base;
            rez.push_back(a%left.base);
        }
        if (delt>0) rez.push_back(delt);
        reverse(rez.begin(),rez.end());
        return LNum(rez, s);
    }

LNum& operator+=(LNum& left, LNum& right)
    {
        left = left + right;
        return left;
    }

ostream& operator<<(ostream& os, LNum &a)
    {
        a.cut0();
        if (a.size()==0 || (a.size()==1 && a.get0()==0)) {os << 0; return os;}
        if (a.sign==0) os<< "-";
        vector <int> p = a.num;
        for (vector <int>::iterator it=p.begin(); it!=p.end(); it++)
            if (it!=p.begin()) os << int_to_str(*it, log10(a.base)); else os << *it; // COULD MADE IT SIMPLY BY PRINTF "%03d"
        return os;
    }

istream& operator>>(istream& is, LNum &a)
    {
        string s;
        is >> s;
        a = s;
        return is;
    }

bool operator == (LNum& x, LNum& y)
    {
        int i=0;
        if (x.sign==y.sign && x.size()==y.size())
        {
            while (i<x.size() && x.geti(i)==y.geti(i)) i++;
            if (i>=x.size()) return true;
        }
        return false;
    }

bool operator == (LNum& x, int y)
    {
        if (y==0 && (x.size()==0 || (x.size()==1 && x.get0()==0))) return true;
        LNum cur (y);
        return x == cur;
    }

bool operator < (LNum& x, LNum& y)
{
    if (x.sign==0 && y.sign==1) return true;
        if (x.sign==1 && y.sign==0) return false;
        if (x.sign> 0)
        {
            if (x.size()<y.size()) return true;
            if (x.size()>y.size()) return false;
            int i=0;
            while (i<x.size() && x.geti(i)==y.geti(i)) i++;
            if (i>=x.size()) return false;
            if (x.geti(i)>y.geti(i)) return false;
        } else
        {
            if (x.size()>y.size()) return true;
            if (x.size()<y.size()) return false;
            int i=0;
            while (i<x.size() && x.geti(i)==y.geti(i)) i++;
            if (i>=x.size()) return false;
            if (x.geti(i)<y.geti(i)) return false;
        }
        return true;
}

bool operator > (LNum& x, LNum& y)
{
    if (x==y || x<y) return false;
    return true;
}

bool operator >= (LNum& x, LNum& y)
{
    if (x<y) return false;
    return true;
}

bool operator <= (LNum& x, LNum& y)
{
    if (x>y) return false;
    return true;
}

const LNum operator*(LNum left, LNum right)
{
    bool s_rez = (left.sign+right.sign + 1)%2;
    left.sign=1;
    right.sign=1;
    LNum rez (0);
    if (left<right) swap(left,right);
    int p=0;
        for (int i=right.size()-1; i>=0; i--)
        {
            LNum cur = left;
            int q=right.geti(i);
            cur = cur.MultNum(q);
            cur.pow10(p); // plus p zero in the end
            p++;
            rez+= cur;
        }
    rez.sign=s_rez;
    return rez;
}

const LNum operator/(LNum left, LNum right)
{
    if (left == right) return LNum(1);
    if (right == 1) return left;
    bool s_rez = (left.sign+right.sign + 1)%2;
    if (left.size()==1 && left.get0()==0) return LNum("0");
    left.sign=1;
    right.sign=1;
    if (left < right) return LNum("");
    int n=right.size();
    LNum cur = left.take_first_N(n);
    if (cur<right) cur = left.take_first_N(++n);
    //cout << "firstN = " << cur << endl;
    //cout << "cur = " << cur << " n= " << n << endl;
    int p=1;
    LNum q = cur-right;
    while (q >= right)
    {
        q=q - right;
        p++;
    }
    vector <int> rez;
    rez.push_back(p);
    //cout << "p = " << p << " q = " << q << endl;
    if (p==1 && q==0) for (int i=0; i<n-1; i++) rez.push_back(0);
    //cout << "afterFirstN = " << left.take_after_first_N(n) << endl;
    q.add_to_end(left.take_after_first_N(n));
    //cout << "new number = " << q << endl;
    //cout << "rez = " << rez << endl;
    LNum result (rez);
    result.sign=s_rez;
    if (q==0) return result;
    LNum next = q/right;
    result.add_to_end(next);
    return result;
}

const LNum operator%(LNum left, LNum right)
{
    bool s_rez = (left.sign+right.sign + 1)%2;
    left.sign=1;
    right.sign=1;
    if (left<right) return left;
    LNum D = left/right;
    LNum rez = right*D;
    rez = left - rez;
    rez.sign = s_rez;
    return rez;
}

LNum gcd (LNum a, LNum b)
{
    a.sign = 1;
    b.sign = 1;
    if (a<b) swap(a,b);
    if (b==0) return a;
    return gcd(b, a%b);
}


/******************************************************
*******************************************************
**************** RATIONAL   NUMBER ********************
*******************************************************
*******************************************************
*******************************************************/

class rational
{
public:
    bool sign=1; // 0 -    1 +
    LNum up=0;
    LNum down=1;

public:
    rational()
    {
        up="0";
        down="1";
        sign = 1;
    }

    rational(LNum _up, LNum _down)
    {
        if (_down==0)
        {
            cerr << "Error in rational (LNum, LNum): up = " << _up << " down = " << _down << endl;
            cerr << "Incorrect down value\n";
            return;
        }
        sign = (_up.sign + _down.sign + 1)%2;
        _up.sign = 1;
        _down.sign = 1;
        up = _up;
        down = _down;
        //cout << "In rational (LNum, Lnum) before cut:\t" << "up = " << up << "  down = " << down << endl;
        cut();
        //cout << "In rational (LNum, Lnum) after cut:\t" << "up = " << up << "  down = " << down << endl;
    }

    rational (int _up, int _down)
    {
        LNum _UP(_up);
        LNum _DOWN(_down);
        sign = (_UP.sign + _DOWN.sign + 1)%2;
        _UP.sign = 1;
        _DOWN.sign = 1;
        up = _UP;
        down = _DOWN;
    }

    rational (int _up)
    {
        if (_up<0) {sign = 0; _up=-_up;}
        LNum _UP(_up);
        LNum _DOWN(1);
        up = _UP;
        down = _DOWN;
        //cout << "In rational (int):\t" << "up = " << up << "  down = " << down << endl;
    }

    void cut ()
    {
        LNum d = gcd(up, down);
        //cout << "in gcd up = " << up << " down = " << down << " d = " << d << endl;
        up = up/d;
        //cout << "up / d = " << up << endl;
        down = down/d;
        //cout << "down / d = " << down << endl;
    }

    friend const rational operator+(rational left, rational right);
    friend const rational operator-(rational left, rational right);
    friend const rational operator*(rational left, rational right);
    friend const rational operator/(rational left, rational right);
    friend const rational operator-(const rational& a);
    friend bool operator < (rational& left, rational& right);
    friend bool operator < (double& left, rational& right);
    friend bool operator < (rational& left, double& right);
    friend ostream& operator<<(ostream& os, rational &a);
    friend istream& operator>>(istream& is, rational &a);
    rational& operator=(const long &a)
    {
        down = 1;
        if (a>0)  {up = a; sign = 1; }
            else {up = -a; sign = 0; }
        //cout << up << " " << down << endl;
        return *this;
    }

    operator double ()
    {
        LNum c (0);
        LNum d (0);
        c = up / down;
        d = up % down;
        double res = c;
        res += double(d) / double (down);
        if (!sign) res*=-1;
        return res;
    }
};

const rational operator+(rational left, rational right)
{
    if (left.sign == 0 && right.sign == 0)
    {
        left.sign = 1;
        right.sign = 1;
        rational rez = left + right;
        rez.sign = 0;
        return rez;
    }
    if (left.sign == 1 && right.sign == 0)
    {
        right.sign = 1;
        rational rez = left - right;
        return rez;
    }
    if (left.sign == 0 && right.sign == 1)
    {
        left.sign = 1;
        cout << left.sign << " " << right.sign << endl;
        rational rez = right - left;
        return rez;
    }
    LNum DOWN = left.down * right.down;
    LNum UP1 = left.up*right.down;
    LNum UP2 = left.down*right.up;
    LNum UP = UP1 + UP2;
    return rational(UP, DOWN);
}

const rational operator-(rational left, rational right)
{
    if (left.sign == 1 && right.sign == 0)
    {
        right.sign = 1;
        rational rez = left + right;
        return rez;
    }
    if (left.sign == 0 && right.sign == 1)
    {
        left.sign = 1;
        rational rez = left + right;
        rez.sign = 0;
        return rez;
    }
    if (left.sign == 0 && right.sign == 0)
    {
        right.sign = 1;
        left.sign = 1;
        rational rez = right - left;
        return rez;
    }
    LNum DOWN = left.down * right.down;
    LNum UP1 = left.up*right.down;
    LNum UP2 = left.down*right.up;
    LNum UP = UP1 - UP2;
    return rational(UP, DOWN);
}

const rational operator*(rational left, rational right)
{
    LNum DOWN = left.down * right.down;
    LNum UP = left.up * right.up;
    rational rez(UP, DOWN);
    rez.sign = (left.sign + right.sign +1)%2;
    rez.cut();
    return rez;
}

const rational operator/(rational left, rational right)
{
    //cout << "MAKING OPERATION " << left << "   /   " << right << endl;
    LNum DOWN = left.down * right.up;
    LNum UP = left.up * right.down;
    //cout << "Rez before cut = " << UP << " / " << DOWN << endl;
    rational rez(UP, DOWN);
    rez.sign = (left.sign + right.sign + 1)%2;
    //cout << "Rez after cut = " << rez << endl << endl << endl;
    return rez;
}

const rational operator-(const rational& a)
{
    rational b = a;
    b.sign == 0 ? b.sign = 1 : b.sign = 0;
    return b;
}

bool operator < (rational& left, rational& right)
{
    return double(left) < double (right);
}

bool operator < (double& left, rational& right)
{
    return left < double (right);
}

bool operator < (rational& left, double& right)
{
    return double(left) < right;
}

ostream& operator<<(ostream& os, rational &a)
    {
        a.cut();
        if (a.sign==0) os<< "-";
        if (a.up==0) os << 0; else if (a.down==1) os << a.up; else os << a.up << " / " << a.down;
        return os;
    }

istream& operator>>(istream& is, rational &a) // two string UP and DOWN
    {
        string s;
        is >> s;
        a.up = s;
        is >> s;
        a.down = s;
        return is;
    }

rational abs(rational a)
{
    return a>=0 ? a : -a;
}

/******************************************************
*******************************************************
********************* MATRIX **************************
*******************************************************
*******************************************************
*******************************************************/

class matrix
{
public:
    vector<vector<rational>> data; // matrix n*m
    int n, m;
public:
    matrix(vector<vector<rational>> &a)
    {
        data = a;
        n = data.size();
        m = data[0].size();
    }

    friend const matrix operator+(matrix left, matrix right);
    friend const matrix operator-(matrix left, matrix right);
    friend const matrix operator*(matrix left, matrix right);
    friend ostream& operator<<(ostream& os, const matrix &a);

    bool is_square ()
    {
        return n==m;
    }
    // Gauss Method ******************************************************************************************************
    bool add_column_to_end (vector <rational> &a)
    {
        if (n!=a.size())
        {
            cerr << "\nError! Incorrect input. Can not add column in matrix.\n";
            return 0;
        }
        vector<vector<rational>> rezult;
        int i=0;
        for (vector<vector<rational>>::iterator it = data.begin(); it!=data.end(); it++)
        {
            vector<rational> cur = *it;
            cur.push_back (a[i]);
            i++;
        }
        data = rezult;
        n++;
        return 1;
    }
    // The End of Gauss Method ###########################################################################################
};

const matrix operator+(matrix left, matrix right)
{
    if (left.n!=right.n || left.m!=right.m)
    {
        cerr << "Error! Incorrect input. Can not plus matrix./n";
        return left;
    }
    int n=left.n, m=left.m;
    vector<vector<rational>> a(n, vector<rational>(m));
    for (int i=0; i<n; i++)
        for (int j=0; j<m; j++)
    {
        rational p =left.data[i][j]+right.data[i][j];
        //cout << left.data[i][j] << " " << right.data[i][j] << endl;
        //cout << p << endl;
        a[i][j]=p;
    }
    return matrix(a);
}

const matrix operator-(matrix left, matrix right)
{
    if (left.n!=right.n || left.m!=right.m)
    {
        cerr << "Error! Incorrect input. Can not minus matrix./n";
        return left;
    }
    int n=left.n, m=left.m;
    vector<vector<rational>> a(n, vector<rational>(m));
    for (int i=0; i<n; i++)
        for (int j=0; j<m; j++)
    {
        a[i][j]=left.data[i][j]-right.data[i][j];
    }
    return matrix(a);
}

const matrix operator*(matrix left, matrix right)
{
    if (left.m!=right.n)
    {
        cerr << "\nError! Incorrect input. Can not multiple matrix.\nAmount of column first matrix should be equal amount of lines second matrix.\nThe result of operation now is first matrix.\n";
        return left;
    }
    int n=left.n, m=right.m;
    vector<vector<rational>> a(n, vector<rational>(m));
    for (int i=0; i<n; i++)
        for (int j=0; j<m; j++)
    {
        rational sum;
        for (int k=0; k<left.m; k++)
        {
            rational d = left.data[i][k]*right.data[k][j];
            sum = sum + d;
        }
        a[i][j]=sum;
    }
    return matrix(a);
}

ostream& operator<<(ostream& os, matrix &a)
{
        for (int i=0; i<a.n; i++)
        {
            for (int j=0; j<a.m-1; j++) os << a.data[i][j] << "  \t";
            os << a.data[i][a.m-1] << endl;
        }
        return os;
    }

int gauss(matrix a, vector <rational> y, vector<rational> &ans) {
  if (!a.is_square()) { cerr << "Matrix is not square\n"; return 0;}
  if (a.n!=y.size()) { cerr << "Size of matrix != size of vector solutions"; return 0;}
  rational M;
  int n = a.n;
  vector <rational> x (n);
  int k, index;
  const double eps = 0.00001;  // точность
  k = 0;
  while (k < n) {
  // Поиск строки с максимальным a[i][k]
    M = abs(a.data[k][k]);
    index = k;
    for (int i = k + 1; i < n; i++) {
      if (abs(a.data[i][k]) > M) {
        M = abs(a.data[i][k]);
        index = i;
      }
    }
  // Перестановка строк
    if (M < eps) {
    // нет ненулевых диагональных элементов
      cout << "Can't get solution because of the empty column " << index << endl;
      return 0;
    }
    for (int j = 0; j < n; j++) {
      rational temp = a.data[k][j];
      a.data[k][j] = a.data[index][j];
      a.data[index][j] = temp;
    }
    rational temp = y[k];
    y[k] = y[index];
    y[index] = temp;
    //cout << "After moving:\n" << a << endl;
    //for (vector <rational>::iterator it = y.begin(); it!= y.end(); it++) cout << *it << "\t";
    //cout << endl << endl;
  // Нормализация уравнений
    for (int i = k; i < n; i++) {
      rational temp = a.data[i][k];
      //cout << "temp = " << temp << endl;
      if (abs(temp) < eps) continue; // для нулевого коэффициента пропустить
      for (int j = k; j < n; j++) {
        //cout << a.data[i][j] << "   /   " << temp << "   =   ";
        a.data[i][j] = a.data[i][j] / temp;
        //cout << a.data[i][j] << endl;
      }
      //cout << y[i] << "   /   " << temp << "   =   ";
      y[i] = y[i] / temp;
      //cout << y[i] << endl;
      if (i == k)  continue; // уравнение не вычитать само из себя
      for (int j = 0; j < n; j++) {
            a.data[i][j] = (a.data[i][j] - a.data[k][j])*temp;
      }
      y[i] = (y[i] - y[k])*temp;
    }
    k++;
    //cout << "After normalization and minus:\n" << a << endl;
    //for (vector <rational>::iterator it = y.begin(); it!= y.end(); it++) cout << *it << "\t";
    //cout << endl << endl;
  }
  //cout << "\t\tAT THE END\n";
  // обратная подстановка
  for (k = n - 1; k >= 0; k--) {
    x[k] = y[k];
    for (int i = 0; i < k; i++) {
      y[i] = y[i] - a.data[i][k] * x[k];
    }
  }
  ans = x;
  return 1;
}

void print_gauss_sol (matrix &a, vector <rational> &y, vector <rational> &x)
{
    cout << "For set of equations:\n";
    for (int i=0; i<a.n; i++)
        {
            for (int j=0; j<a.n-1; j++)
            {
                cout << a.data[i][j] << "*x[" << j << "] + ";
            }
            cout << a.data[i][a.n-1] << "*x[" << a.n-1 << "] = ";
            cout << y[i] << endl;
        }
    cout << "\nSolutions are:\n";
    for (int i=0; i<x.size(); i++) cout << "x[" << i << "] = " << double(x[i]) << endl;
}
int main()
{
    srand (time(NULL));
    cout << "\n****************** LONG   NUMBER **********************\n";
    // Test of long number operators + - * (uncomment next code)
/*
    LNum a (0);
    LNum b (0);
    LNum c (0);
    LNum d (0);
    LNum e (0);
    cout << "Input a, b:\n";
    cin >> a >> b;
    cout << "a = " << a << ", b = " << b << endl;
    c = a+b;
    cout << a << " + " << b << " = "  << c << endl;
    c = a-b;
    cout << a << " - " << b << " = "  << c << endl;
    c = c.Karatsuba(a,b);
    cout << a << " *KARATSUBA* " << b << " = "  << c << endl;
    c = a*b;
    cout << a << " * " << b << " = "  << c << endl;
    c = a/b;
    cout << a << " / " << b << " = "  << c << endl;
    c = gcd(a,b);
    cout << "gcd(a,b) = " << c << endl;
    c = a * a;
    d = b * b;
    c = c - d; // c = a^2 - b^2;
    d = a - b;
    e = a + b;
    d = d * e; // d = (a-b) * (a+b)
    bool t = (c==d);
    cout << "Result of operation \"a^2 - b^2 = (a-b) * (a+b)\" is:\n" << c << " = " << d << "  -> " << t << endl;
*/
    cout << "\n**************** RATIONAL   NUMBER ********************\n";
    // Test of rational operators + - * (uncomment next code)
/*
    LNum la (7);
    LNum lb (13);
    rational a(la, lb);
    cout << "a = " << a << " <=> " << double(a) << endl;
    LNum lc (50);
    LNum ld (70);
    rational b(lc,ld);
    cout << "b = " << b << " <=> " << double(b) << endl;
    rational c (0);
    rational d (0);
    rational e (0);
    c = a+b;
    cout << a << " + " << b << " = "  << c << " <=> " << double(c) << endl;
    c = a-b;
    cout << a << " - " << b << " = "  << c << " <=> " << double(c) << endl;
    c = a*b;
    cout << a << " * " << b << " = "  << c << " <=> " << double(c) << endl;
    c = a/b;
    cout << a << " / " << b << " = "  << c << " <=> " << double(c) << endl;
    c = a * a;
    d = b * b;
    c = c - d; // c = a^2 - b^2;
    d = a - b;
    e = a + b;
    d = d * e; // d = (a-b) * (a+b)
    bool t = (c==d);
    cout << "Result of operation \"a^2 - b^2 = (a-b) * (a+b)\" is:\n" << c << " = " << d << "  -> " << t << endl;
*/
    cout << "\n********************* MATRIX **************************\n";
    // Test of matrix operators + - * (uncomment next code)
/*
    vector<vector<rational>> first;
    for (int i=0; i<4; i++)
    {
        vector <rational> cur;
        for (int j=0; j<4; j++)
        {
            cur.push_back(rational(rand()%30 + 1, rand()%30 + 1));
        }
        first.push_back(cur);
    }
    vector<vector<rational>> second;
    for (int i=0; i<4; i++)
    {
        vector <rational> cur;
        for (int j=0; j<4; j++)
        {
            cur.push_back(rational(rand()%30 + 1, rand()%30 + 1));
        }
        second.push_back(cur);
    }
    matrix q (first);
    cout <<"\nFirst matrix:\n" << q;
    matrix w (second);
    cout <<"\nSecond matrix:\n" << w;
    matrix e(first);
    e = q + w;
    cout <<"\nTheir summary:\n" << e;
    e = q - w;
    cout <<"\nTheir difference:\n" << e;
    e = q * w;
    cout <<"\nTheir multiplication:\n" << e;
*/

    // Test of Gauss method (uncomment next code)

    vector<vector<rational>> third;
    vector<vector<rational>> X;
    vector<rational> x1;
    vector<rational> cur;
    for (int i=0; i<4; i++)
        {
            x1.push_back(i+1);
            X.push_back(x1);
            x1.erase(x1.begin(), x1.end());
            for (int j=0; j<4; j++)
            {
                cur.push_back(rational(rand()%30+1));
            }
            third.push_back(cur);
            cur.erase(cur.begin(), cur.end());
        }
    matrix x(X);
    matrix a (third);
    matrix B (X);
    B = a*x;
    vector<rational> solution;
    for (int i=0; i<B.n; i++) solution.push_back(B.data[i][0]);
    vector <rational> answers;
    cout << "Vector that go after sign =\n";
    for (vector <rational>::iterator it = solution.begin(); it!=solution.end(); it++) cout << *it << " ";
    cout << "\nMatrix\n" << a << endl;
    if (gauss(a, solution, answers))
        print_gauss_sol(a, solution, answers);

    return 0;
}
