#ifndef SIMPLEOBJECTS_H
#define SIMPLEOBJECTS_H 1

#include <vector>

/**
 * @breif Identifier class
 */
class Identifier {
    protected: 
        int m_fileId;
        int m_eventId;
        int m_uid;
    public:
        /**
         * @breif Default Constructor
         */
        Identifier() : m_fileId(-1), m_eventId(-1), m_uid(-1) {}

        /**
         * @breif Parametrised Constructor
         *
         * @param fileId the file identification integer
         * @param eventId the event identification integer
         * @param uid the unique identifier for the object
         */
        Identifier(int fileId, int eventId, int uid) : m_fileId(fileId), m_eventId(eventId), m_uid(uid) {}

        /**
         * @brief Copy assignment operator
         */
        Identifier & operator=(const Identifier & id);

        /**
         * @breif Default Destructor
         */
        ~Identifier() {}

        // Getters
        // -----------------------------------------
        int GetFileId() const;
        int GetEventId() const;
        int GetUid() const;

        // Setters
        // -----------------------------------------
        /*
        void SetFileId(int fileId);
        void SetEventId(int eventId);
        void SetUid(int uid);
        */

        // Overload the comparison operator to compare Identifiers
        bool operator==(const Identifier &id);
        friend bool operator== ( const Identifier &n1, const Identifier &n2);

};

// -------------------------------------------------------------

Identifier & Identifier::operator=(const Identifier & id){
    if (this != &id ){
        m_fileId = id.m_fileId;
        m_eventId = id.m_eventId;
        m_uid = id.m_uid;
    }
    return *this;
}

// Getters
int Identifier::GetFileId()  const { return this->m_fileId; }
int Identifier::GetEventId() const { return this->m_eventId;}
int Identifier::GetUid()     const { return this->m_uid;    }

// Setters
/*
void Identifier::SetFileId(int fileId)   { this->m_fileId  = fileId; }
void Identifier::SetEventId(int eventId) { this->m_eventId = eventId;}
void Identifier::SetUid(int uid)         { this->m_uid     = uid;    }
*/

bool Identifier::operator==(const Identifier &id){
    return (m_fileId  == id.GetFileId()  &&
            m_eventId == id.GetEventId() &&
            m_uid     == id.GetUid()     );
}
bool operator== ( const Identifier &n1, const Identifier &n2) {
    return (n1.GetFileId()  == n2.GetFileId()  &&
            n1.GetEventId() == n2.GetEventId() &&
            n1.GetUid()     == n2.GetUid()     );
}

// -------------------------------------------------------------
// End of Identifier class
// -------------------------------------------------------------





/**
 * @breif SimpleCaloHit class
 */
class SimpleCaloHit{
    protected:
        Identifier m_id;
        char m_view;
        double m_x;
        double m_z;
        bool m_isRemoved;
        bool m_isNeutrinoInduced;
        Identifier m_mcParticleId;
        Identifier m_pfoId;
    public:
        /**
         * @breif Default Constructor
         */
        SimpleCaloHit() : m_view('X'), m_x(std::numeric_limits<double>::max()), m_z(std::numeric_limits<double>::max()), m_isRemoved(true), m_isNeutrinoInduced(false) {}

        /**
         * @breif Parametrised Constructor
         */
        SimpleCaloHit(Identifier id, char view, double x, double z, bool isRemoved, Identifier mcParticleId, Identifier pfoId, bool isNeutrinoInduced) : m_id(id), m_view(view), m_x(x), m_z(z), m_isRemoved(isRemoved), m_mcParticleId(mcParticleId), m_pfoId(pfoId), m_isNeutrinoInduced(isNeutrinoInduced) {}

        /**
         * @brief Copy Constructor
         */
        //SimpleCaloHit(SimpleCaloHit const& hit);

        /**
         * @brief Copy Assignment Operator
         */
        //SimpleCaloHit & operator=(SimpleCaloHit const& hit);

        /**
         * @breif Destructor
         */
        ~SimpleCaloHit() {}

        // Assignment operator
        SimpleCaloHit & operator=( const SimpleCaloHit & other); 

        // Getters
        Identifier GetId()             const;
        char       GetView()           const;
        double     GetX()              const;
        double     GetZ()              const;
        bool       IsRemoved()         const;
        bool       IsNeutrinoInduced() const;
        Identifier GetMCParticleId()   const;
        Identifier GetPfoId()          const;

        // Setters
        /*
        void SetId(Identifier id);
        void SetView(char view);
        void SetX(double x);
        void SetZ(double z);
        */
        void SetRemoved(bool isRemoved);

        // Overload the comparison operator to compare Identifiers
        bool operator==(const SimpleCaloHit &other);
        friend bool operator== ( const SimpleCaloHit &n1, const SimpleCaloHit &n2);


};
// -------------------------------------------------------------

// Assignment operator
SimpleCaloHit & SimpleCaloHit::operator=( const SimpleCaloHit & other) {
    if (this != &other){
        m_id = other.m_id;
        m_view = other.m_view;
        m_x = other.m_x;
        m_z = other.m_z;
        m_isRemoved = other.m_isRemoved;
        m_isNeutrinoInduced = other.m_isNeutrinoInduced;
        m_mcParticleId = other.m_mcParticleId;
        m_pfoId = other.m_pfoId;
    }
    return *this;
}

// Getters
Identifier SimpleCaloHit::GetId()             const { return m_id;               }
char       SimpleCaloHit::GetView()           const { return m_view;             }
double     SimpleCaloHit::GetX()              const { return m_x;                }
double     SimpleCaloHit::GetZ()              const { return m_z;                }
bool       SimpleCaloHit::IsRemoved()         const { return m_isRemoved;        }
bool       SimpleCaloHit::IsNeutrinoInduced() const { return m_isNeutrinoInduced;}
Identifier SimpleCaloHit::GetMCParticleId()   const { return m_mcParticleId;     }
Identifier SimpleCaloHit::GetPfoId()          const { return m_pfoId;            }

// Setters
/*
void SimpleCaloHit::SetId(Identifier id)       { m_id        = id;        } 
void SimpleCaloHit::SetView(char view)         { m_view      = view;      }
void SimpleCaloHit::SetX(double x)             { m_x         = x;         }
void SimpleCaloHit::SetZ(double z)             { m_z         = z;         }
*/
void SimpleCaloHit::SetRemoved(bool isRemoved) { m_isRemoved = isRemoved; }

bool SimpleCaloHit::operator==(const SimpleCaloHit &other){
    return (m_id == other.GetId());
}

bool operator== ( const SimpleCaloHit &n1, const SimpleCaloHit &n2) {
        return (n1.GetId() == n2.GetId());
}

// -------------------------------------------------------------
// End of SimpleCaloHit class
// -------------------------------------------------------------





/**
 * @breif SimplePfo class
 */
class SimplePfo{
    protected:
        Identifier m_id;
        std::vector<Identifier> m_hitList;
        bool m_isRemoved;
        double m_fractionRemoved;
    public:
        /**
         * @breif Default Constructor
         */
        SimplePfo() : m_isRemoved(false) {}

        /**
         * @breif Parametrised Constructor
         */
        SimplePfo(Identifier id, std::vector<Identifier> hitList, bool isRemoved, double fractionRemoved) : m_id(id), m_isRemoved(isRemoved), m_hitList(hitList), m_fractionRemoved(fractionRemoved) {}

        /**
         * @breif Destructor
         */
        ~SimplePfo() {}

        // Getters 
        Identifier               GetId()              const;
        bool                     IsRemoved()          const;
        double                   GetFractionRemoved() const;
        std::vector<Identifier>& GetHitList();
        
        // Setters
        void SetRemoved(bool isRemoved);

};

// Getters
Identifier                SimplePfo::GetId()              const { return m_id;        }
bool                      SimplePfo::IsRemoved()          const { return m_isRemoved; }
double                    SimplePfo::GetFractionRemoved() const { return m_fractionRemoved; }
std::vector<Identifier> & SimplePfo::GetHitList()               { return m_hitList;   }

// Setters
void SimplePfo::SetRemoved(bool isRemoved) { m_isRemoved = isRemoved; }

// -------------------------------------------------------------
// End of SimplePfo class
// -------------------------------------------------------------





/**
 * @breif SimpleMCParticle class
 */
class SimpleMCParticle{
    protected:
        Identifier              m_id;
        Identifier              m_primaryVisibleNeutrinoId;
        int                     m_pdg;
        bool                    m_isNeutrinoInduced;
        std::vector<Identifier> m_hitList;
        double                  m_startX;
        double                  m_startY;
        double                  m_startZ;
        double                  m_endX;
        double                  m_endY;
        double                  m_endZ;
    public:
        /**
         * @breif Default Constructor
         */
        SimpleMCParticle() : m_pdg(std::numeric_limits<int>::max())       , 
                             m_isNeutrinoInduced(false)                   , 
                             m_startX(std::numeric_limits<double>::max()) ,
                             m_startY(std::numeric_limits<double>::max()) ,
                             m_startZ(std::numeric_limits<double>::max()) , 
                             m_endX(std::numeric_limits<double>::max())   , 
                             m_endY(std::numeric_limits<double>::max())   , 
                             m_endZ(std::numeric_limits<double>::max())   {}
        /**
         * @breif Parametrised Constructor
         */
        SimpleMCParticle(Identifier id, int pdg, bool isNeutrinoInduced, std::vector<Identifier> hitList, double startX, double startY, double startZ, double endX, double endY, double endZ) : 
                             m_id(id)                               ,
                             m_pdg(pdg)                             , 
                             m_isNeutrinoInduced(isNeutrinoInduced) , 
                             m_hitList(hitList)                     ,
                             m_startX(startX)                       ,
                             m_startY(startY)                       ,
                             m_startZ(startZ)                       , 
                             m_endX(endX)                           , 
                             m_endY(endY)                           , 
                             m_endZ(endZ)                           ,
                             m_primaryVisibleNeutrinoId(Identifier(id.GetFileId(), id.GetEventId(), -1)) {}

        /**
         * @breif Parametrised Constructor accepting primary visible neutrino id
         */
        SimpleMCParticle(Identifier id, int pdg, bool isNeutrinoInduced, std::vector<Identifier> hitList, double startX, double startY, double startZ, double endX, double endY, double endZ, Identifier primaryVisibleNeutrinoId) : 
                             m_id(id)                               ,
                             m_pdg(pdg)                             , 
                             m_isNeutrinoInduced(isNeutrinoInduced) , 
                             m_hitList(hitList)                     ,
                             m_startX(startX)                       ,
                             m_startY(startY)                       ,
                             m_startZ(startZ)                       , 
                             m_endX(endX)                           , 
                             m_endY(endY)                           , 
                             m_endZ(endZ)                           ,
                             m_primaryVisibleNeutrinoId(primaryVisibleNeutrinoId) {}

        /**
         * @breif Destructor
         */
        ~SimpleMCParticle() {}

        // Assignment operator
        SimpleMCParticle & operator=( const SimpleMCParticle & other); 

        // Getters 
        Identifier               GetId()             const;
        Identifier               GetPrimaryVisibleNeutrinoId() const;
        int                      GetPdg()            const;
        bool                     IsNeutrinoInduced() const;
        std::vector<Identifier>& GetHitList();
        double                   GetStartX()         const;
        double                   GetStartY()         const;
        double                   GetStartZ()         const;
        double                   GetEndX()           const;
        double                   GetEndY()           const;
        double                   GetEndZ()           const;
        
};

// Assignment operator
SimpleMCParticle & SimpleMCParticle::operator=( const SimpleMCParticle & other) {
    if (this != &other){
        m_id = other.m_id;
        m_primaryVisibleNeutrinoId = other.m_primaryVisibleNeutrinoId;
        m_pdg = other.m_pdg;
        m_isNeutrinoInduced = other.m_isNeutrinoInduced;
        m_hitList = other.m_hitList;
        m_startX = other.m_startX;
        m_startY = other.m_startY;
        m_startZ = other.m_startZ;
        m_endX = other.m_endX;
        m_endY = other.m_endY;
        m_endZ = other.m_endZ;
    }
    return *this;
}

// Getters
Identifier                SimpleMCParticle::GetId()             const { return m_id;                }
Identifier                SimpleMCParticle::GetPrimaryVisibleNeutrinoId() const { return m_primaryVisibleNeutrinoId; }
int                       SimpleMCParticle::GetPdg()            const { return m_pdg;               }
bool                      SimpleMCParticle::IsNeutrinoInduced() const { return m_isNeutrinoInduced; }
std::vector<Identifier> & SimpleMCParticle::GetHitList()              { return m_hitList;           }
double                    SimpleMCParticle::GetStartX()         const { return m_startX;            }
double                    SimpleMCParticle::GetStartY()         const { return m_startY;            }
double                    SimpleMCParticle::GetStartZ()         const { return m_startZ;            }
double                    SimpleMCParticle::GetEndX()           const { return m_endX;              }
double                    SimpleMCParticle::GetEndY()           const { return m_endY;              }
double                    SimpleMCParticle::GetEndZ()           const { return m_endZ;              }

// -------------------------------------------------------------
// End of SimpleMCParticle class
// -------------------------------------------------------------





/**
 * @breif SimpleMCEvent class
 */
class SimpleMCEvent{
    protected:
        int                           m_fileId;
        int                           m_eventId;
        std::vector<SimpleCaloHit>    m_caloHitList;
        std::vector<SimplePfo>        m_pfoList;
        std::vector<SimpleMCParticle> m_mcParticleList;
    public:
        /**
         * @breif Default Constructor
         */
        SimpleMCEvent() : m_fileId(-1), m_eventId(-1) {}

        /**
         * @breif Parametrised Constructor
         */
        SimpleMCEvent(int fileId, int eventId) : m_fileId(fileId), m_eventId(eventId) {}

        /**
         * @bried Destructor
         */
        ~SimpleMCEvent() {}

        // Getters
        int                             GetFileId()         const;
        int                             GetEventId()        const;
        std::vector<SimpleCaloHit>    & GetCaloHitList();
        std::vector<SimplePfo>        & GetPfoList();
        std::vector<SimpleMCParticle> & GetMCParticleList();

        // Setters
        void SetFileId(int fileId);
        void SetEventId(int eventId);
        void AddCaloHit(SimpleCaloHit &caloHit);
        void AddPfo(SimplePfo &pfo);
        void AddMCParticle(SimpleMCParticle &mcParticle);
};

// Getters
int                             SimpleMCEvent::GetFileId()         const { return m_fileId;         }
int                             SimpleMCEvent::GetEventId()        const { return m_eventId;        }
std::vector<SimpleCaloHit>    & SimpleMCEvent::GetCaloHitList()          { return m_caloHitList;    }
std::vector<SimplePfo>        & SimpleMCEvent::GetPfoList()              { return m_pfoList;        }
std::vector<SimpleMCParticle> & SimpleMCEvent::GetMCParticleList()       { return m_mcParticleList; }

// Setters
void SimpleMCEvent::SetFileId(int fileId)                       { m_fileId  = fileId;                     }
void SimpleMCEvent::SetEventId(int eventId)                     { m_eventId = eventId;                    }
void SimpleMCEvent::AddCaloHit(SimpleCaloHit &caloHit)          { m_caloHitList.push_back(caloHit);       }
void SimpleMCEvent::AddPfo(SimplePfo &pfo)                      { m_pfoList.push_back(pfo);               }
void SimpleMCEvent::AddMCParticle(SimpleMCParticle &mcParticle) { m_mcParticleList.push_back(mcParticle); }

// -------------------------------------------------------------
// End of SimpleMCEvent class
// -------------------------------------------------------------

#endif // #ifndef SIMPLEOBJECTS_H
