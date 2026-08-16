#pragma once

namespace HydraFramework
{
#pragma pack(push, 1)
    struct TypeID {
    private:
        static const int IsCashItemOffset = 0;
        static const int IsCashItemSize = 1;
        static const unsigned short IsCashItemMask = (1 << IsCashItemSize) - 1 << IsCashItemOffset;

        static const int IsBionicOffset = IsCashItemOffset + IsCashItemSize;
        static const int IsBionicSize = 1;
        static const unsigned short IsBionicMask = (1 << IsBionicSize) - 1 << IsBionicOffset;

        static const int FirstTidOffset = IsBionicOffset + IsBionicSize;
        static const int FirstTidSize = 3;
        static const unsigned short FirstTidMask = (1 << FirstTidSize) - 1 << FirstTidOffset;

        static const int SecondTidOffset = FirstTidOffset + FirstTidSize;
        static const int SecondTidSize = 2;
        static const unsigned short SecondTidMask = (1 << SecondTidSize) - 1 << SecondTidOffset;

        static const int ThirdTidOffset = SecondTidOffset + SecondTidSize;
        static const int ThirdTidSize = 4;
        static const unsigned short ThirdTidMask = (1 << ThirdTidSize) - 1 << ThirdTidOffset;

        static const int FourthTidOffset = ThirdTidOffset + ThirdTidSize;
        static const int FourthTidSize = 5;
        static const unsigned short FourthTidMask = (1 << FourthTidSize) - 1 << FourthTidOffset;

    public:
        unsigned short Value;

        // IsCashItem Get / Set
        bool GetIsCashItem() const
        {
            return (this->Value & IsCashItemMask) >> IsCashItemOffset > 0;
        }

        void SetIsCashItem(bool value)
        {
            unsigned short val = value ? 1 : 0;
            this->Value = (unsigned short)(this->Value & ~IsCashItemMask | val << IsCashItemOffset & IsCashItemMask);
        }

        // IsBionic Get / Set
        bool GetIsBionic() const
        {
            return (this->Value & IsBionicMask) >> IsBionicOffset > 0;
        }
        void SetIsBionic(bool value)
        {
            unsigned short val = value ? 1 : 0;
            this->Value = (unsigned short)(this->Value & ~IsBionicMask | val << IsBionicOffset & IsBionicMask);
        }

        // First Get / Set
        unsigned char GetFirst() const
        {
            return (unsigned char)((this->Value & FirstTidMask) >> FirstTidOffset);
        }
        void SetFirst(unsigned char value)
        {
            unsigned short val = value & (1 << FirstTidSize) - 1;
            this->Value = (unsigned short)(this->Value & ~FirstTidMask | val << FirstTidOffset & FirstTidMask);
        }

        // Second Get / Set
        unsigned char GetSecond() const
        {
            return (unsigned char)((this->Value & SecondTidMask) >> SecondTidOffset);
        }

        void SetSecond(unsigned char value)
        {
            unsigned short val = value & (1 << SecondTidSize) - 1;
            this->Value = (unsigned short)(this->Value & ~SecondTidMask | val << SecondTidOffset & SecondTidMask);
        }

        // Third Get / Set
        unsigned char GetThird() const
        {
            return (unsigned char)((this->Value & ThirdTidMask) >> ThirdTidOffset);
        }

        void SetThird(unsigned char value)
        {
            unsigned short val = value & (1 << ThirdTidSize) - 1;
            this->Value = (unsigned short)(this->Value & ~ThirdTidMask | val << ThirdTidOffset & ThirdTidMask);
        }

        // Fourth Get / Set
        unsigned char GetFourth() const
        {
            return (unsigned char)((this->Value & FourthTidMask) >> FourthTidOffset);
        }

        void SetFourth(unsigned char value)
        {
            unsigned short val = value & (1 << FourthTidSize) - 1;
            this->Value = (unsigned short)(this->Value & ~FourthTidMask | val << FourthTidOffset & FourthTidMask);
        }

        // Constructors
        TypeID() : Value(0) {}

        explicit TypeID(unsigned short value) : Value(value) {}

        TypeID(bool isCashItem, bool isBionic, unsigned char first, unsigned char second, unsigned char third, unsigned char fourth)
            : Value(0)
        {
            SetIsCashItem(isCashItem);
            SetIsBionic(isBionic);
            SetFirst(first);
            SetSecond(second);
            SetThird(third);
            SetFourth(fourth);
        }

        // Operators & Methods
        bool Equals(const TypeID& other) const
        {
            return this->Value == other.Value;
        }

        bool operator==(const TypeID& other) const
        {
            return Equals(other);
        }

        bool operator!=(const TypeID& other) const
        {
            return !Equals(other);
        }

        int GetHashCode() const
        {
            return static_cast<int>(Value);
        }

        std::string ToString() const
        {
            std::stringstream ss;
            ss << "IsCashItem = " << (GetIsCashItem() ? "true" : "false") << "\n";
            ss << "IsBionic = " << (GetIsBionic() ? "true" : "false") << "\n";
            ss << "First = " << static_cast<unsigned int>(GetFirst()) << "\n";
            ss << "Second = " << static_cast<unsigned int>(GetSecond()) << "\n";
            ss << "Third = " << static_cast<unsigned int>(GetThird()) << "\n";
            ss << "Fourth = " << static_cast<unsigned int>(GetFourth()) << "\n";
            return ss.str();
        }

        // Implicit Conversion Operator
        operator unsigned short() const
        {
            return Value;
        }
    };
#pragma pack(pop)

    struct RefObjCommon {
        friend class CRefDataManager;

    private:
        long m_ID;
        char m_CodeName128[129];
        char m_ObjName128[129];
        char m_OrgObjCodeName128[129];
        char m_NameStrID128[129];
        char m_DescStrID128[129];
        TypeID m_TypeID;
        long m_DecayTime;
        char m_Country;
        char m_Rarity;
        char m_CanTrade;
        char m_CanSell;
        char m_CanBuy;
        char m_CanBorrow;
        char m_CanDrop;
        char m_CanPick;
        char m_CanRepair;
        char m_CanRevive;
        char m_CanUse;
        char m_CanThrow;
        long m_Price;
        long m_CostRepair;
        long m_CostRevive;
        long m_CostBorrow;
        long m_KeepingFee;
        long m_SellPrice;
        long m_ReqLevelType1;
        char m_ReqLevel1;
        long m_ReqLevelType2;
        char m_ReqLevel2;
        long m_ReqLevelType3;
        char m_ReqLevel3;
        long m_ReqLevelType4;
        char m_ReqLevel4;
        long m_MaxContain;
        RegionInfo m_RegionInfo;
        short m_Speed1;
        short m_Speed2;
        long m_Scale;
        short m_BCHeight;
        short m_BCRadius;
        long m_EventID;
        char m_AssocFileObj128[129];
        char m_AssocFileDrop128[129];
        char m_AssocFileIcon128[129];
        char m_AssocFile1_128[129];
        char m_AssocFile2_128[129];

    protected:
        RefObjCommon() : m_ID(0), m_DecayTime(0), m_Country(0), m_Rarity(0), m_CanTrade(0), m_CanSell(0), m_CanBuy(0),
                         m_CanBorrow(0), m_CanDrop(0), m_CanPick(0), m_CanRepair(0), m_CanRevive(0), m_CanUse(0),
                         m_CanThrow(0), m_Price(0), m_CostRepair(0), m_CostRevive(0), m_CostBorrow(0), m_KeepingFee(0),
                         m_SellPrice(0), m_ReqLevelType1(0), m_ReqLevel1(0), m_ReqLevelType2(0), m_ReqLevel2(0),
                         m_ReqLevelType3(0), m_ReqLevel3(0), m_ReqLevelType4(0), m_ReqLevel4(0), m_MaxContain(0),
                         m_Speed1(0), m_Speed2(0), m_Scale(0), m_BCHeight(0), m_BCRadius(0), m_EventID(0)
        {
            ZeroMemory(m_CodeName128, sizeof(m_CodeName128));
            ZeroMemory(m_ObjName128, sizeof(m_ObjName128));
            ZeroMemory(m_OrgObjCodeName128, sizeof(m_OrgObjCodeName128));
            ZeroMemory(m_NameStrID128, sizeof(m_NameStrID128));
            ZeroMemory(m_DescStrID128, sizeof(m_DescStrID128));
            ZeroMemory(m_AssocFileObj128, sizeof(m_AssocFileObj128));
            ZeroMemory(m_AssocFileDrop128, sizeof(m_AssocFileDrop128));
            ZeroMemory(m_AssocFileIcon128, sizeof(m_AssocFileIcon128));
            ZeroMemory(m_AssocFile1_128, sizeof(m_AssocFile1_128));
            ZeroMemory(m_AssocFile2_128, sizeof(m_AssocFile2_128));
        }

        bool Is(char a1, char a2, char a3, char a4) const
        {
            return (m_TypeID.GetFirst() == a1 &&
                    m_TypeID.GetSecond() == a2 &&
                    m_TypeID.GetThird() == a3 &&
                    m_TypeID.GetFourth() == a4);
        }

    public:
        long ObjID() const { return m_ID; }
        const char* CodeName128() const { return m_CodeName128; }
        const char* ObjName128() const { return m_ObjName128; }
        const char* OrgObjCodeName128() const { return m_OrgObjCodeName128; }
        const char* NameStrID128() const { return m_NameStrID128; }
        const char* DescStrID128() const { return m_DescStrID128; }
        long DecayTime() const { return m_DecayTime; }
        char Country() const { return m_Country; }
        char Rarity() const { return m_Rarity; }
        char CanTrade() const { return m_CanTrade; }
        char CanSell() const { return m_CanSell; }
        char CanBuy() const { return m_CanBuy; }
        char CanBorrow() const { return m_CanBorrow; }
        char CanDrop() const { return m_CanDrop; }
        char CanPick() const { return m_CanPick; }
        char CanRepair() const { return m_CanRepair; }
        char CanRevive() const { return m_CanRevive; }
        char CanUse() const { return m_CanUse; }
        char CanThrow() const { return m_CanThrow; }
        long Price() const { return m_Price; }
        long CostRepair() const { return m_CostRepair; }
        long CostRevive() const { return m_CostRevive; }
        long CostBorrow() const { return m_CostBorrow; }
        long KeepingFee() const { return m_KeepingFee; }
        long SellPrice() const { return m_SellPrice; }
        long ReqLevelType1() const { return m_ReqLevelType1; }
        char ReqLevel1() const { return m_ReqLevel1; }
        long ReqLevelType2() const { return m_ReqLevelType2; }
        char ReqLevel2() const { return m_ReqLevel2; }
        long ReqLevelType3() const { return m_ReqLevelType3; }
        char ReqLevel3() const { return m_ReqLevel3; }
        long ReqLevelType4() const { return m_ReqLevelType4; }
        char ReqLevel4() const { return m_ReqLevel4; }
        long MaxContain() const { return m_MaxContain; }
        const RegionInfo &RegionInfo() const { return m_RegionInfo; }
        short Speed1() const { return m_Speed1; }
        short Speed2() const { return m_Speed2; }
        long Scale() const { return m_Scale; }
        short BCHeight() const { return m_BCHeight; }
        short BCRadius() const { return m_BCRadius; }
        long EventID() const { return m_EventID; }
        const char* AssocFileObj128() const { return m_AssocFileObj128; }
        const char* AssocFileDrop128() const { return m_AssocFileDrop128; }
        const char* AssocFileIcon128() const { return m_AssocFileIcon128; }
        const char* AssocFile1_128() const { return m_AssocFile1_128; }
        const char* AssocFile2_128() const { return m_AssocFile2_128; }

        TypeID GetTypeID() const { return m_TypeID; }

        bool IsPC() const { return Is(1,1,0,0); }

        bool IsMob() const {
            return (Is(1,2,1,0) ||
                    Is(1,2,1,1) ||
                    (Is(1,2,1,4) && m_DecayTime != 0) ||
                    Is(1,2,3,6) ||
                    Is(1,2,3,7) ||
                    Is(1,2,3,8));
        }

        bool IsMobThief() const {
            return Is(1,2,1,2);
        }

        bool IsMobHunter() const {
            return Is(1,2,1,3);
        }

        bool IsJobNPC() const {
            return (IsMobThief() || IsMobHunter());
        }

        bool IsStructure() const {
            return (Is(1,2,1,4) || Is(1,2,1,8) || Is(1,2,4,2));
        }

        bool IsMobEvenPandora() const {
            return Is(1,2,1,5); //One and only
        }

        bool IsTradeCOS_UNK() const {
            return Is(1,2,1,6);
        }

        bool IsTradeCOSThief() const {
            return Is(1,2,1,7); //?
        }

        bool IsNPC() const {
            return Is(1,2,2,0);
        }

        bool IsStructPulley() const {
            return Is(1,2,2,1);
        }

        bool IsNPCBossDungeon() const {
            return (Is(1,2,2,2) || Is(1,2,2,3));
        }

        bool IsCOS() const {
            return (Is(1,2,3,0) || Is(1,2,3,1));
        }

        bool IsTradeCOS() const {
            return Is(1,2,3,2);
        }

        bool IsPet() const {
            return Is(1,2,3,3);
        }

        bool IsGrabPet() const {
            return Is(1,2,3,4);
        }

        bool IsMercenary() const {
            return Is(1,2,3,5);
        }

        bool IsFellowPet() const {
            return Is(1,2,3,9);
        }

        bool IsSiegeCOS() const {
            return (Is(1,2,3,9) || Is(1,2,3,10));
        }

        bool IsSiegeGuardCOS() const {
            return (Is(1,2,4,1) || Is(1,2,4,4));
        }

        bool IsSiegeUniques() const {
            return Is(1,2,4,3);
        }

        bool IsSiegeStructHeart() const {
            return Is(1,2,5,1);
        }

        bool IsSiegeStructTower() const {
            return Is(1,2,5,2);
        }

        bool IsSiegeStructGate() const {
            return Is(1,2,5,3);
        }

        bool IsSiegeStructCommandCamp() const {
            return Is(1,2,5,4);
        }

        bool IsSiegeStructDefensivePos() const {
            return Is(1,2,5,5);
        }

        bool IsSiegeStructBarricade() const {
            return Is(1,2,5,6);
        }

        bool IsMuhanTower() const {
            return Is(1,2,6,0);
        }

        bool IsMuhanTreasureBox() const {
            return Is(1,2,7,0);
        }

        bool IsSurvivalArenaSafeZone() const {
            return Is(1,2,8,0);
        }

        bool IsArmor() const {
            return (Is(3,1,1,1) ||
                    Is(3,1,1,2) ||
                    Is(3,1,1,3) ||
                    Is(3,1,1,4) ||
                    Is(3,1,1,5) ||
                    Is(3,1,1,6) ||
                    Is(3,1,2,1) ||
                    Is(3,1,2,2) ||
                    Is(3,1,2,3) ||
                    Is(3,1,2,4) ||
                    Is(3,1,2,5) ||
                    Is(3,1,2,6) ||
                    Is(3,1,3,1) ||
                    Is(3,1,3,2) ||
                    Is(3,1,3,3) ||
                    Is(3,1,3,4) ||
                    Is(3,1,3,5) ||
                    Is(3,1,3,6) ||
                    Is(3,1,9,1) ||
                    Is(3,1,9,2) ||
                    Is(3,1,9,3) ||
                    Is(3,1,9,4) ||
                    Is(3,1,9,5) ||
                    Is(3,1,9,6) ||
                    Is(3,1,10,1) ||
                    Is(3,1,10,2) ||
                    Is(3,1,10,3) ||
                    Is(3,1,10,4) ||
                    Is(3,1,10,5) ||
                    Is(3,1,10,6) ||
                    Is(3,1,11,1) ||
                    Is(3,1,11,2) ||
                    Is(3,1,11,3) ||
                    Is(3,1,11,4) ||
                    Is(3,1,11,5) ||
                    Is(3,1,11,6)
            );
        }

        bool IsShield() const {
            return (Is(3,1,4,1) ||
                    Is(3,1,4,2)
            );
        }

        bool IsAccessory() const {
            return (Is(3,1,5,1) ||
                    Is(3,1,5,2) ||
                    Is(3,1,5,3) ||
                    Is(3,1,12,1) ||
                    Is(3,1,12,2) ||
                    Is(3,1,12,3)
            );
        }

        bool IsWeapon() const {
            return (Is(3,1,6,2) ||
                    Is(3,1,6,3) ||
                    Is(3,1,6,4) ||
                    Is(3,1,6,5) ||
                    Is(3,1,6,6) ||
                    Is(3,1,6,7) ||
                    Is(3,1,6,8) ||
                    Is(3,1,6,9) ||
                    Is(3,1,6,10) ||
                    Is(3,1,6,11) ||
                    Is(3,1,6,12) ||
                    Is(3,1,6,13) ||
                    Is(3,1,6,14) ||
                    Is(3,1,6,15) ||
                    Is(3,1,6,16)
            );
        }

        bool IsEquip() const {
            return (IsArmor() || IsShield() || IsAccessory() || IsWeapon());
        }

        bool IsExtra() const {
            return (Is(3,1,7,1) ||
                    Is(3,1,7,2) ||
                    Is(3,1,7,3) ||
                    Is(3,1,7,5) ||
                    Is(3,1,7,6) ||
                    Is(3,1,7,7)
            );
        }

        bool IsAvatar() const {
            return (Is(3,1,13,1) ||
                    Is(3,1,13,2) ||
                    Is(3,1,13,3) ||
                    Is(3,1,13,4) //FLAG
            );
        }

        bool IsSpiritBracelet() const {
            return (Is(3,1,14,1));
        }

        bool IsSummonScroll() const {
            return (Is(3,2,1,1) ||
                    Is(3,2,1,2) ||
                    Is(3,2,1,3)
            );
        }

        bool IsExpendables() const {
            return (Is(3,3,1,1) ||
                    Is(3,3,1,2) ||
                    Is(3,3,1,3) ||
                    Is(3,3,1,4) ||
                    Is(3,3,1,6) ||
                    Is(3,3,1,8) ||
                    Is(3,3,1,9) ||
                    Is(3,3,1,10) ||
                    Is(3,3,1,11) ||
                    Is(3,3,1,12) ||
                    Is(3,3,1,13) ||
                    Is(3,3,2,1) ||
                    Is(3,3,2,6) ||
                    Is(3,3,2,7) ||
                    Is(3,3,2,8) ||
                    Is(3,3,2,9) ||
                    Is(3,3,3,1) ||
                    Is(3,3,3,2) ||
                    Is(3,3,3,3) ||
                    Is(3,3,3,4) ||
                    Is(3,3,3,5) ||
                    Is(3,3,3,6) ||
                    Is(3,3,3,7) ||
                    Is(3,3,3,8) ||
                    Is(3,3,3,9) ||
                    Is(3,3,3,10) ||
                    Is(3,3,3,11) ||
                    Is(3,3,3,12) ||
                    Is(3,3,3,13) ||
                    Is(3,3,3,14) ||
                    Is(3,3,3,15) ||
                    Is(3,3,3,16) ||
                    Is(3,3,3,17) ||
                    Is(3,3,3,18) ||
                    Is(3,3,3,19) ||
                    Is(3,3,3,20) ||
                    Is(3,3,3,21) ||
                    Is(3,3,3,22) ||
                    Is(3,3,3,23) ||
                    Is(3,3,4,1) ||
                    Is(3,3,4,2) ||
                    Is(3,3,5,0) ||
                    Is(3,3,5,1) ||
                    Is(3,3,6,1) ||
                    Is(3,3,6,2) ||
                    Is(3,3,7,1) ||
                    Is(3,3,8,0) ||
                    Is(3,3,8,1) ||
                    Is(3,3,8,2) ||
                    Is(3,3,8,3) ||
                    Is(3,3,9,0) ||
                    Is(3,3,9,1) ||
                    Is(3,3,11,1) ||
                    Is(3,3,11,2) ||
                    Is(3,3,11,3) ||
                    Is(3,3,11,4) ||
                    Is(3,3,11,5) ||
                    Is(3,3,11,6) ||
                    Is(3,3,11,7) ||
                    Is(3,3,11,8) ||
                    Is(3,3,11,9) ||
                    Is(3,3,11,10) ||
                    Is(3,3,11,11) ||
                    Is(3,3,11,12) ||
                    Is(3,3,11,13) ||
                    Is(3,3,11,14) ||
                    Is(3,3,11,15) ||
                    Is(3,3,11,17) ||
                    Is(3,3,11,18) ||
                    Is(3,3,12,1) ||
                    Is(3,3,12,2) ||
                    Is(3,3,12,3) ||
                    Is(3,3,12,4) ||
                    Is(3,3,12,5) ||
                    Is(3,3,12,6) ||
                    Is(3,3,12,7) ||
                    Is(3,3,12,8) ||
                    Is(3,3,12,9) ||
                    Is(3,3,12,10) ||
                    Is(3,3,13,0) ||
                    Is(3,3,13,1) ||
                    Is(3,3,13,4) ||
                    Is(3,3,13,5) ||
                    Is(3,3,13,6) ||
                    Is(3,3,13,7) ||
                    Is(3,3,13,8) ||
                    Is(3,3,13,9) ||
                    Is(3,3,13,10) ||
                    Is(3,3,13,11) ||
                    Is(3,3,13,12) ||
                    Is(3,3,13,13) ||
                    Is(3,3,13,14) ||
                    Is(3,3,13,15) ||
                    Is(3,3,13,16) ||
                    Is(3,3,13,17) ||
                    Is(3,3,13,18) ||
                    Is(3,3,13,19) ||
                    Is(3,3,13,20) ||
                    Is(3,3,13,21) ||
                    Is(3,3,13,22) ||
                    Is(3,3,13,23) ||
                    Is(3,3,13,24) ||
                    Is(3,3,13,25) ||
                    Is(3,3,13,26) ||
                    Is(3,3,13,30) ||
                    Is(3,3,13,31) ||
                    Is(3,3,14,1) ||
                    Is(3,3,14,2) ||
                    Is(3,3,14,3) ||
                    Is(3,3,14,4) ||
                    Is(3,3,14,5) ||
                    Is(3,3,15,1) ||
                    Is(3,3,15,2) ||
                    Is(3,3,16,1) ||
                    Is(3,3,16,3) ||
                    Is(3,3,16,4) ||
                    Is(3,3,16,5)
            );
        }

        bool IsEquipment() const {
            return (IsArmor() || IsShield() || IsAccessory() || IsWeapon());
        }

        bool IsJobEquipment() const {
            return (Is(3,4,1,1) ||
                    Is(3,4,1,2) ||
                    Is(3,4,1,3) ||
                    Is(3,4,1,4) ||
                    Is(3,4,1,5) ||
                    Is(3,4,1,6) ||
                    Is(3,4,2,1) ||
                    Is(3,4,3,1) ||
                    Is(3,4,3,2) ||
                    Is(3,4,3,3) ||
                    Is(3,4,4,1) ||
                    Is(3,4,4,2) ||
                    Is(3,4,4,3) ||
                    Is(3,4,4,4) ||
                    Is(3,4,4,5) ||
                    Is(3,4,4,6) ||
                    Is(3,4,5,1) ||
                    Is(3,4,6,1) ||
                    Is(3,4,6,2) ||
                    Is(3,4,6,3)
            );
        }

        bool IsSpecialty() const {
            return (Is(3,3,10,1) ||
                    Is(3,3,10,2) ||
                    Is(3,3,10,4) ||
                    Is(3,3,10,5) ||
                    Is(3,3,10,6) ||
                    Is(3,3,10,8) ||
                    Is(3,3,10,9) ||
                    Is(3,3,10,10) ||
                    Is(3,3,10,11)
            );
        }
    };
}
