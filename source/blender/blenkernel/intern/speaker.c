/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** \file
 * \ingroup bke
 */

#include "DNA_defaults.h"
#include "DNA_object_types.h"
#include "DNA_sound_types.h"
#include "DNA_speaker_types.h"

#include "BLI_math.h"
#include "BLI_utildefines.h"

#include "BLT_translation.h"

#include "BKE_anim_data.h"
#include "BKE_idtype.h"
#include "BKE_lib_id.h"
#include "BKE_lib_query.h"
#include "BKE_main.h"
#include "BKE_speaker.h"

#include "BLO_read_write.h"

static void speaker_init_data(ID *id)
{
  Speaker *speaker = (Speaker *)id;

  BLI_assert(MEMCMP_STRUCT_AFTER_IS_ZERO(speaker, id));

  MEMCPY_STRUCT_AFTER(speaker, DNA_struct_default_get(Speaker), id);
}

static void speaker_foreach_id(ID *id, LibraryForeachIDData *data)
{
  Speaker *speaker = (Speaker *)id;

  BKE_LIB_FOREACHID_PROCESS(data, speaker->sound, IDWALK_CB_USER);
}

static void speaker_blend_write(BlendWriter *writer, ID *id, const void *id_address)
{
  Speaker *spk = (Speaker *)id;
  if (spk->id.us > 0 || BLO_write_is_undo(writer)) {
    /* write LibData */
    BLO_write_id_struct(writer, Speaker, id_address, &spk->id);
    BKE_id_blend_write(writer, &spk->id);

    if (spk->adt) {
      BKE_animdata_blend_write(writer, spk->adt);
    }
  }
}

static void speaker_blend_read_data(BlendDataReader *reader, ID *id)
{
  Speaker *spk = (Speaker *)id;
  BLO_read_data_address(reader, &spk->adt);
  BKE_animdata_blend_read_data(reader, spk->adt);

#if 0
  spk->sound = newdataadr(fd, spk->sound);
  direct_link_sound(fd, spk->sound);
#endif
}

static void speaker_blend_read_lib(BlendLibReader *reader, ID *id)
{
  Speaker *spk = (Speaker *)id;
  BLO_read_id_address(reader, spk->id.lib, &spk->sound);
}

static void speaker_blend_read_expand(BlendExpander *expander, ID *id)
{
  Speaker *spk = (Speaker *)id;
  BLO_expand(expander, spk->sound);
}

IDTypeInfo IDType_ID_SPK = {
    .id_code = ID_SPK,
    .id_filter = FILTER_ID_SPK,
    .main_listbase_index = INDEX_ID_SPK,
    .struct_size = sizeof(Speaker),
    .name = "Speaker",
    .name_plural = "speakers",
    .translation_context = BLT_I18NCONTEXT_ID_SPEAKER,
    .flags = 0,

    .init_data = speaker_init_data,
    .copy_data = NULL,
    .free_data = NULL,
    .make_local = NULL,
    .foreach_id = speaker_foreach_id,
    .foreach_cache = NULL,

    .blend_write = speaker_blend_write,
    .blend_read_data = speaker_blend_read_data,
    .blend_read_lib = speaker_blend_read_lib,
    .blend_read_expand = speaker_blend_read_expand,
};

void *BKE_speaker_add(Main *bmain, const char *name)
{
  Speaker *spk;

  spk = BKE_libblock_alloc(bmain, ID_SPK, name, 0);

  speaker_init_data(&spk->id);

  return spk;
}

Speaker *BKE_speaker_copy(Main *bmain, const Speaker *spk)
{
  Speaker *spk_copy;
  BKE_id_copy(bmain, &spk->id, (ID **)&spk_copy);
  return spk_copy;
}
