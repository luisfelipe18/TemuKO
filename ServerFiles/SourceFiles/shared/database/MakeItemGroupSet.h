#pragma once

class CMakeItemGroupSet : public OdbcRecordset
{
public:
	CMakeItemGroupSet(OdbcConnection * dbConnection, MakeItemGroupArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MAKE_ITEM_GROUP"); }
	virtual tstring GetColumns() { return _T("iItemGroupNum, iItem_1, iItem_2, iItem_3, iItem_4, iItem_5, iItem_6, iItem_7, iItem_8, iItem_9, iItem_10, iItem_11, iItem_12, iItem_13, iItem_14, iItem_15, iItem_16, iItem_17, iItem_18, iItem_19, iItem_20, iItem_21, iItem_22, iItem_23, iItem_24, iItem_25, iItem_26, iItem_27, iItem_28, iItem_29, iItem_30, iItem_31, iItem_32, iItem_33, iItem_34, iItem_35, iItem_36, iItem_37, iItem_38, iItem_39, iItem_40, iItem_41, iItem_42, iItem_43, iItem_44, iItem_45, iItem_46, iItem_47, iItem_48, iItem_49, iItem_50, iItem_51, iItem_52, iItem_53, iItem_54, iItem_55, iItem_56, iItem_57, iItem_58, iItem_59, iItem_60, iItem_61, iItem_62, iItem_63, iItem_64, iItem_65, iItem_66, iItem_67, iItem_68, iItem_69, iItem_70, iItem_71, iItem_72, iItem_73, iItem_74, iItem_75, iItem_76, iItem_77, iItem_78, iItem_79, iItem_80, iItem_81, iItem_82, iItem_83, iItem_84, iItem_85, iItem_86, iItem_87, iItem_88, iItem_89, iItem_90, iItem_91, iItem_92, iItem_93, iItem_94, iItem_95, iItem_96, iItem_97, iItem_98, iItem_99, iItem_100, iItem_101, iItem_102, iItem_103, iItem_104, iItem_105, iItem_106, iItem_107, iItem_108, iItem_109, iItem_110, iItem_111, iItem_112, iItem_113, iItem_114, iItem_115, iItem_116, iItem_117, iItem_118, iItem_119, iItem_120, iItem_121, iItem_122, iItem_123, iItem_124, iItem_125, iItem_126, iItem_127, iItem_128, iItem_129, iItem_130, iItem_131, iItem_132, iItem_133, iItem_134, iItem_135, iItem_136, iItem_137, iItem_138, iItem_139, iItem_140, iItem_141, iItem_142, iItem_143, iItem_144, iItem_145, iItem_146, iItem_147, iItem_148, iItem_149, iItem_150, iItem_151, iItem_152, iItem_153, iItem_154, iItem_155, iItem_156, iItem_157, iItem_158, iItem_159, iItem_160, iItem_161, iItem_162, iItem_163, iItem_164, iItem_165, iItem_166, iItem_167, iItem_168, iItem_169, iItem_170, iItem_171, iItem_172, iItem_173, iItem_174, iItem_175, iItem_176, iItem_177, iItem_178, iItem_179, iItem_180, iItem_181, iItem_182, iItem_183, iItem_184, iItem_185, iItem_186, iItem_187, iItem_188, iItem_189, iItem_190, iItem_191, iItem_192, iItem_193, iItem_194, iItem_195, iItem_196, iItem_197, iItem_198, iItem_199, iItem_200"); }

	virtual bool Fetch()
	{
		_MAKE_ITEM_GROUP *pData = new _MAKE_ITEM_GROUP;

		_dbCommand->FetchUInt32(1, pData->iItemGroupNum);
		for (int i = 1; i <= 200; i++)
		{
			uint32 iItem;
			_dbCommand->FetchUInt32(i + 1, iItem);

			if (iItem == 0)
				continue;

			// Insert regardless of whether it's set or not. This is official behaviour.
			pData->iItems.push_back(iItem);
		}

		if (!m_pMap->PutData(pData->iItemGroupNum, pData))
			delete pData;

		return true;
	}

	MakeItemGroupArray *m_pMap;
};