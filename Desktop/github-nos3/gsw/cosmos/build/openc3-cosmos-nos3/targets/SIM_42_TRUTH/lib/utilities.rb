
module OpenC3
    class Utilities
        def self.dot(u, v)
            return u[0]*v[0] + u[1]*v[1] + u[2]*v[2]
        end

        def self.norm(u)
            return Math.sqrt(dot(u,u))
        end

        def self.cross(u, v)
            w = []
            w[0] = u[1]*v[2]-u[2]*v[1]
            w[1] = u[2]*v[0]-u[0]*v[2]
            w[2] = u[0]*v[1]-u[1]*v[0]
            return w
        end

        def self.sxv(s, v)
            r = []
            r[0] = s*v[0]
            r[1] = s*v[1]
            r[2] = s*v[2]
            return r
        end

        def self.uaddv(u, v)
            r = []
            r[0] = u[0] + v[0]
            r[1] = u[1] + v[1]
            r[2] = u[2] + v[2]
            return r
        end

        def self.Q2C(q)
            twoQ00 = 2.0*q[0]*q[0];
            twoQ11 = 2.0*q[1]*q[1];
            twoQ22 = 2.0*q[2]*q[2];
            twoQ01 = 2.0*q[0]*q[1];
            twoQ02 = 2.0*q[0]*q[2];
            twoQ03 = 2.0*q[0]*q[3];
            twoQ12 = 2.0*q[1]*q[2];
            twoQ13 = 2.0*q[1]*q[3];
            twoQ23 = 2.0*q[2]*q[3];

            c = [[1,0,0],[0,1,0],[0,0,1]]
            c[0][0] = 1.0-(twoQ11+twoQ22);
            c[0][1] = twoQ01+twoQ23;
            c[0][2] = twoQ02-twoQ13;
            c[1][0] = twoQ01-twoQ23;
            c[1][1] = 1.0-(twoQ22+twoQ00);
            c[1][2] = twoQ12+twoQ03;
            c[2][0] = twoQ02+twoQ13;
            c[2][1] = twoQ12-twoQ03;
            c[2][2] = 1.0-(twoQ00+twoQ11);
            return c
        end

        def self.MxV (m, v)
            w = []
            w[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];
            w[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];
            w[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];
            return w
        end

        def self.MTxV (m, v)
            w = []
            w[0] = m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2];
            w[1] = m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2];
            w[2] = m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2];
            return w
        end
        
    end
end
