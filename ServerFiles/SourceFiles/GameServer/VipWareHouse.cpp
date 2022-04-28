#include "stdafx.h"
#include "DBAgent.h"
/**
* @brief	Handles requests related to VIP Storage.
*/
void CUser::ReqVipStorageProcess(Packet & pkt)
{
	Packet result(WIZ_VIPWAREHOUSE);
	uint8 bOpcode = pkt.read<uint8>();

	if (isDead() || !isInGame())
	{
		result << bOpcode << uint8(2);
		Send(&result);
		return;
	}

	switch (bOpcode)
	{
	case VIP_UseVault: // Use Vault Item
	{
		uint16 sNpcId;
		uint32 nItemID;
		uint8 page, bSrcPos, bDstPos;
		_ITEM_TABLE * pTable = nullptr;
		_ITEM_DATA * pSrcItem = nullptr;
		CNpc * pNpc = nullptr;
		uint32 Days;

		pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos;
		pNpc = g_pMain->GetNpcPtr(sNpcId, GetZoneID());
		if (pNpc == nullptr
			|| pNpc->GetType() != NPC_WAREHOUSE
			|| !isInRange(pNpc, MAX_NPC_RANGE))
		{
			result << uint8(VIP_UseVault) << uint8(2);
			Send(&result);
			return;
		}

		if (nItemID != VIP_VAULT_KEY
			&& nItemID != VIP_SAFE_KEY_1
			&& nItemID != VIP_SAFE_KEY_7)
		{
			result << uint8(VIP_UseVault) << uint8(2);
			Send(&result);
			return;
		}

		pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr)
		{
			result << uint8(VIP_UseVault) << uint8(2);
			Send(&result);
			return;
		}

		pSrcItem = GetItem(SLOT_MAX + bSrcPos);
		if (pSrcItem == nullptr)
		{
			result << uint8(VIP_UseVault) << uint8(2);
			Send(&result);
			return;
		}

		if (nItemID == VIP_VAULT_KEY)
			Days = 30;
		else if (nItemID == VIP_SAFE_KEY_1)
			Days = 1;
		else if (nItemID == VIP_SAFE_KEY_7)
			Days = 7;
		else
		{
			result << uint8(VIP_UseVault) << uint8(2);
			Send(&result);
			return;
		}

		// Check for invalid slot IDs.
		if (bSrcPos > HAVE_MAX
			// Cannot be traded, sold or stored (note: don't check the race, as these items CAN be stored).
			|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| pSrcItem->nNum != nItemID
			// Rented items cannot be placed in the inn.
			|| pSrcItem->isRented()
			|| pSrcItem->isDuplicate())
		{
			result << uint8(VIP_UseVault) << uint8(2);
			Send(&result);
			return;
		}

		uint32 iexpiration = uint32(UNIXTIME + 60 * 60 * 24 * Days);
		uint8 bSealResult = g_DBAgent.VIPStorageUseVaultKey(GetAccountName(), iexpiration);
		if (bSealResult != 1)
		{
			result << uint8(VIP_UseVault) << uint8(2);
			Send(&result);
			return;
		}

		memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		m_bVIPStorageVaultExpiration = iexpiration;
		result << uint8(VIP_UseVault) << uint8(1) << uint8(1) << m_bVIPStorageVaultExpiration;
		Send(&result);
	}
	break;
	case VIP_SetPassword: // Set Password
	{
		std::string strSealPasswd;
		pkt >> strSealPasswd;

		uint8 bSealResult = g_DBAgent.VIPStorageSetPassword(strSealPasswd, GetAccountName(), 1);
		if (bSealResult != 1)
		{
			result << uint8(VIP_SetPassword) << uint8(2);
			Send(&result);
			return;
		}

		m_bVIPStoragePassword = strSealPasswd;
		m_bWIPStorePassowrdRequest = 1;
		result << uint8(VIP_SetPassword) << bSealResult;
		Send(&result);
	}
	break;
	case VIP_CancelPassword: // cancel password
	{
		uint8 bCancelResult = g_DBAgent.VIPStorageCancelPassword(GetAccountName(), 0);
		if (bCancelResult != 1)
		{
			result << uint8(VIP_CancelPassword) << uint8(2);
			Send(&result);
			return;
		}

		m_bWIPStorePassowrdRequest = 0;
		result << uint8(VIP_CancelPassword) << bCancelResult;
		Send(&result);
	}
	break;
	case VIP_ChangePassword: // Change Password
	{
		std::string strSealPasswd;
		pkt >> strSealPasswd;
		//yeni db komut yazýlacak.
		uint8 bSealResult = g_DBAgent.VIPStorageSetPassword(strSealPasswd, GetAccountName(), m_bWIPStorePassowrdRequest);

		if (bSealResult != 1)
		{
			result << uint8(VIP_ChangePassword) << uint8(2);
			Send(&result);
			return;
		}

		m_bVIPStoragePassword = strSealPasswd;
		result << uint8(VIP_ChangePassword) << bSealResult;
		Send(&result);
	}
	break;
	}
}

#pragma region CUser::VIPhouseProcess(Packet & pkt)
/**
* @brief	Handles the VIP Storage feature packets.
*
* @param	pkt	The packet.
*/
void CUser::VIPhouseProcess(Packet & pkt)
{
	Packet result(WIZ_VIPWAREHOUSE);
	CNpc * pNpc = nullptr;
	uint16 aaNpcID;
	uint8 opcode;
	//uint8 bResult = 2;
	bool isKey = false;

	pkt >> opcode;

	if (isDead()
		|| !isInGame())
	{
		result << opcode << uint8(2);
		Send(&result);
		return;
	}

	if (isTrading()
		|| isMining()
		|| isMerchanting()
		|| isFishing())
	{
		result << opcode << uint8(2);
		Send(&result);
		return;
	}

	if ((m_bVIPStorageVaultExpiration == 0
		|| m_bVIPStorageVaultExpiration < UNIXTIME)
		&& (opcode == VIP_InvenToStorage
			|| opcode == VIP_StorageToStore))
	{
		result << opcode << uint8(2);
		Send(&result);
		return;
	}

	if (opcode == VIP_Open) // VIP Storage Open
	{
		pkt >> aaNpcID;

		std::string password;
		pkt.SByte();
		pkt >> password;

		if (m_bWIPStorePassowrdRequest > 0)
		{
			if (m_bWIPStorePassowrdRequest > 1)
				m_bWIPStorePassowrdRequest = 1;

			if (m_bWIPStorePassowrdRequest < 0)
				m_bWIPStorePassowrdRequest = 0;
		}

		if (UNIXTIME >= m_bVIPStorageVaultExpiration)
			m_bVIPStorageVaultExpiration = 0;

		pNpc = g_pMain->GetNpcPtr(aaNpcID, GetZoneID());
		if (pNpc == nullptr)
		{
			if (m_bVIPStorageVaultExpiration == 0
				|| m_bVIPStorageVaultExpiration < UNIXTIME)
			{
				result << uint8(1) << uint8(21);
				Send(&result);
				return;
			}
		}
		else
		{
			if (pNpc != nullptr)
			{
				if (pNpc->GetType() != NPC_WAREHOUSE
					|| !isInRange(pNpc, MAX_NPC_RANGE))
				{
					result << uint8(1) << uint8(0);
					Send(&result);
					return;
				}
			}
		}

		if (m_bWIPStorePassowrdRequest > 0)
		{
			if (m_bVIPStoragePassword.length() > 0)
			{
				result << uint8(11) << uint8(1);
				Send(&result);
				return;
			}
		}

		if (m_bVIPStorageVaultExpiration != 0)
			isKey = true;

		result << opcode
			<< uint8(1)
			<< uint8(m_bWIPStorePassowrdRequest)
			<< uint8(isKey)
			<< uint32(m_bVIPStorageVaultExpiration)
			<< uint32(VIP_VAULT_KEY)
			<< uint16(1)
			<< uint16(1)
			<< uint8(0)
			<< uint32(0)
			<< uint16(0);

		for (int i = 0; i < VIPWAREHOUSE_MAX; i++)
		{
			_ITEM_DATA *pItem = &m_sVIPWarehouseArray[i];

			if (pItem == nullptr)
				continue;

			result << pItem->nNum
				<< pItem->sDuration
				<< pItem->sCount
				<< pItem->bFlag;

			_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
			if (pItemTable != nullptr)
			{
				if (pItemTable->isPetItem())
					ShowPetItemInfo(result, pItem->nSerialNum);
				else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
					ShowCyperRingItemInfo(result, pItem->nSerialNum);
				else
					result << uint32(0); // Item Unique ID
			}
			else
				result << uint32(0); // Item Unique ID*/

			/*if (pItem->nNum == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pItem->nSerialNum);
			else
				result << uint32(0); // Item Unique ID*/

			result << pItem->nExpirationTime;
		}
		Send(&result);
	}
	else if (opcode == VIP_InvenToStorage) // Inven to VIP Storage
	{
		if (m_bVIPStoragePassword.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		uint16 sNpcId, nCount;
		uint32 nItemID;
		uint8 page, bSrcPos, bDstPos;
		_ITEM_TABLE * pTable = nullptr;
		_ITEM_DATA * pSrcItem = nullptr, *pDstItem = nullptr;
		pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos >> nCount;
		if (nItemID == ITEM_GOLD)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pSrcItem = GetItem(SLOT_MAX + bSrcPos);
		if (pSrcItem == nullptr)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		// Check for invalid slot IDs.
		if (bSrcPos > HAVE_MAX
			|| bDstPos > VIPWAREHOUSE_MAX
			// Cannot be traded, sold or stored (note: don't check the race, as these items CAN be stored).
			|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| pSrcItem->nNum != nItemID
			// Rented items cannot be placed in the inn.
			|| pSrcItem->isRented()
			|| pSrcItem->isDuplicate())
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pDstItem = &m_sVIPWarehouseArray[bDstPos];

		if (pTable->m_bKind == 255
			&& pDstItem->nNum != 0)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable()
			&& pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.
			|| pSrcItem->sCount < nCount)// Ensure users have enough of the specified item to move.
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		if (pTable->m_bKind == 255)
		{
			pDstItem->sCount = (uint16)pSrcItem->sCount;
			pSrcItem->sCount = 0;
		}
		else
		{
			pDstItem->sCount += (uint16)nCount;
			pSrcItem->sCount -= nCount;
		}

		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if ((!pTable->isStackable() || nCount == pDstItem->sCount))
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemMove(1, 1);
		SendItemWeight();

		result << opcode << uint8(1);
		Send(&result);
	}
	else if (opcode == VIP_StorageToInven) // VIP Storage to Inven
	{
		if (m_bVIPStoragePassword.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		uint16 sNpcId, nCount;
		uint32 nItemID;
		uint8 page, bSrcPos, bDstPos;
		_ITEM_TABLE * pTable = nullptr;
		_ITEM_DATA * pSrcItem = nullptr, *pDstItem = nullptr;

		pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos >> nCount;

		if (nItemID == ITEM_GOLD)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		// Ensure we're not being given an invalid slot ID.
		if (bSrcPos > VIPWAREHOUSE_MAX
			|| bDstPos > HAVE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = &m_sVIPWarehouseArray[bSrcPos])->nNum != nItemID
			// Does the player have enough room in their inventory?
			|| !CheckWeight(pTable, nItemID, (uint16)nCount))
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pDstItem = GetItem(SLOT_MAX + bDstPos);
		if (pDstItem == nullptr)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.
			|| pSrcItem->sCount < nCount) // Ensure users have enough of the specified item to move.
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		pDstItem->sCount += (uint16)nCount;
		pSrcItem->sCount -= nCount;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemMove(1, 1);
		SendItemWeight();

		result << opcode << uint8(1);
		Send(&result);
	}
	else if (opcode == VIP_StorageToStore) // VIP Storage to VIP Storage
	{
		if (m_bVIPStoragePassword.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		uint16 sNpcId, nCount;
		uint32 nItemID;
		uint8 page, bSrcPos, bDstPos;
		_ITEM_TABLE * pTable = nullptr;
		_ITEM_DATA * pSrcItem = nullptr, *pDstItem = nullptr;
		pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos >> nCount;

		// Ensure we're not being given an invalid slot ID.
		if (bSrcPos > VIPWAREHOUSE_MAX
			|| bDstPos > VIPWAREHOUSE_MAX)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pSrcItem = &m_sVIPWarehouseArray[bSrcPos];
		pDstItem = &m_sVIPWarehouseArray[bDstPos];

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inn (the whole stack is moved).
			|| pDstItem->nNum != 0)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		result << opcode << uint8(1);
		Send(&result);
	}
	else if (opcode == VIP_InvenToInven) // Inven to Inven Storage
	{
		if (m_bVIPStoragePassword.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		uint16 sNpcId, nCount;
		uint32 nItemID;
		uint8 page, bSrcPos, bDstPos;
		_ITEM_TABLE * pTable = nullptr;
		_ITEM_DATA * pSrcItem = nullptr, *pDstItem = nullptr, tmpItem;
		pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos >> nCount;

		if (bSrcPos > HAVE_MAX
			|| bDstPos > HAVE_MAX)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		pSrcItem = GetItem(SLOT_MAX + bSrcPos);
		pDstItem = GetItem(SLOT_MAX + bDstPos);

		if (pSrcItem == nullptr || pDstItem == nullptr)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
		memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)

		result << opcode << uint8(1);
		Send(&result);
	}
	else if (opcode == VIP_UseVault) // Use Vault Key
	{
		if (m_bVIPStoragePassword.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		if (m_bVIPStorageVaultExpiration - UNIXTIME > 60 * 60 * 24 * 30)
		{
			result << opcode << uint8(5);
			Send(&result);
			return;
		}

		uint16 sNpcId;
		uint32 nItemID;
		uint8 page, bSrcPos, bDstPos;
		pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos;

		if (nItemID != VIP_VAULT_KEY)
		{
			result << opcode << uint8(4);
			Send(&result);
			return;
		}

		if (!CheckExistItem(VIP_VAULT_KEY))
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		result << opcode << sNpcId << nItemID << page << bSrcPos << bDstPos;
		g_pMain->AddDatabaseRequest(result, this);
	}
	else if (opcode == VIP_SetPassword) // Set Password
	{
		std::string password;
		pkt.SByte();
		pkt >> password;

		if (password.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		if (!std::all_of(password.begin(), password.end(), ::isdigit))
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		result << opcode << password;
		g_pMain->AddDatabaseRequest(result, this);
	}
	else if (opcode == VIP_CancelPassword) // Cancel password
	{
		result << opcode;
		g_pMain->AddDatabaseRequest(result, this);
	}
	else if (opcode == VIP_ChangePassword) // Change Password
	{
		std::string password;
		pkt.SByte();
		pkt >> password;

		if (password.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		if (!std::all_of(password.begin(), password.end(), ::isdigit))
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		result << opcode << password;
		g_pMain->AddDatabaseRequest(result, this);
	}
	else if (opcode == VIP_EnterPassword) // Pass login
	{
		std::string password;
		pkt.SByte();
		pkt >> password;

		if (password.length() != 4)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		if (!std::all_of(password.begin(), password.end(), ::isdigit))
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}

		if (password != m_bVIPStoragePassword)
		{
			result << opcode << uint8(2);
			Send(&result);
			return;
		}
		result << opcode << uint8(1) << uint8(0);
		Send(&result);

		result.clear();
		result.Initialize(WIZ_VIPWAREHOUSE);
		result << uint8(1) << uint8(1) << uint8(m_bVIPStoragePassword.length() == 4);

		if (m_bVIPStorageVaultExpiration < UNIXTIME)
			result << uint8(0) << uint32(0);
		else
			result << uint8(1) << uint32(m_bVIPStorageVaultExpiration);

		result << uint32(VIP_VAULT_KEY) << uint16(1) << uint16(1) << uint8(0) << uint32(0) << uint16(0);

		for (int i = 0; i < VIPWAREHOUSE_MAX; i++)
		{
			_ITEM_DATA *pItem = &m_sVIPWarehouseArray[i];

			if (pItem == nullptr)
				continue;

			result << pItem->nNum
				<< pItem->sDuration
				<< pItem->sCount
				<< pItem->bFlag;

			_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
			if (pItemTable != nullptr)
			{
				if (pItemTable->isPetItem())
					ShowPetItemInfo(result, pItem->nSerialNum);
				else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
					ShowCyperRingItemInfo(result, pItem->nSerialNum);
				else
					result << uint32(0); // Item Unique ID
			}
			else
				result << uint32(0); // Item Unique ID*/

			/*if (pItem->nNum == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pItem->nSerialNum);
			else
				result << uint32(0); // Item Unique ID*/

			result << pItem->nExpirationTime;
		}
		Send(&result);
	}
}

#pragma endregion