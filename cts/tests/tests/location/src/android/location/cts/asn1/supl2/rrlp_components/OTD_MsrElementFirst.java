/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.location.cts.asn1.supl2.rrlp_components;

/*
 */


//
//
import android.location.cts.asn1.base.Asn1Integer;
import android.location.cts.asn1.base.Asn1Null;
import android.location.cts.asn1.base.Asn1Object;
import android.location.cts.asn1.base.Asn1Sequence;
import android.location.cts.asn1.base.Asn1Tag;
import android.location.cts.asn1.base.BitStream;
import android.location.cts.asn1.base.BitStreamReader;
import android.location.cts.asn1.base.SequenceComponent;
import com.google.common.collect.ImmutableList;
import java.util.Collection;
import javax.annotation.Nullable;


/**
*/
public  class OTD_MsrElementFirst extends Asn1Sequence {
  //

  private static final Asn1Tag TAG_OTD_MsrElementFirst
      = Asn1Tag.fromClassAndNumber(-1, -1);

  public OTD_MsrElementFirst() {
    super();
  }

  @Override
  @Nullable
  protected Asn1Tag getTag() {
    return TAG_OTD_MsrElementFirst;
  }

  @Override
  protected boolean isTagImplicit() {
    return true;
  }

  public static Collection<Asn1Tag> getPossibleFirstTags() {
    if (TAG_OTD_MsrElementFirst != null) {
      return ImmutableList.of(TAG_OTD_MsrElementFirst);
    } else {
      return Asn1Sequence.getPossibleFirstTags();
    }
  }

  /**
   * Creates a new OTD_MsrElementFirst from encoded stream.
   */
  public static OTD_MsrElementFirst fromPerUnaligned(byte[] encodedBytes) {
    OTD_MsrElementFirst result = new OTD_MsrElementFirst();
    result.decodePerUnaligned(new BitStreamReader(encodedBytes));
    return result;
  }

  /**
   * Creates a new OTD_MsrElementFirst from encoded stream.
   */
  public static OTD_MsrElementFirst fromPerAligned(byte[] encodedBytes) {
    OTD_MsrElementFirst result = new OTD_MsrElementFirst();
    result.decodePerAligned(new BitStreamReader(encodedBytes));
    return result;
  }



  @Override protected boolean isExtensible() {
    return false;
  }

  @Override public boolean containsExtensionValues() {
    for (SequenceComponent extensionComponent : getExtensionComponents()) {
      if (extensionComponent.isExplicitlySet()) return true;
    }
    return false;
  }

  
  private OTD_MsrElementFirst.refFrameNumberType refFrameNumber_;
  public OTD_MsrElementFirst.refFrameNumberType getRefFrameNumber() {
    return refFrameNumber_;
  }
  /**
   * @throws ClassCastException if value is not a OTD_MsrElementFirst.refFrameNumberType
   */
  public void setRefFrameNumber(Asn1Object value) {
    this.refFrameNumber_ = (OTD_MsrElementFirst.refFrameNumberType) value;
  }
  public OTD_MsrElementFirst.refFrameNumberType setRefFrameNumberToNewInstance() {
    refFrameNumber_ = new OTD_MsrElementFirst.refFrameNumberType();
    return refFrameNumber_;
  }
  
  private ModuloTimeSlot referenceTimeSlot_;
  public ModuloTimeSlot getReferenceTimeSlot() {
    return referenceTimeSlot_;
  }
  /**
   * @throws ClassCastException if value is not a ModuloTimeSlot
   */
  public void setReferenceTimeSlot(Asn1Object value) {
    this.referenceTimeSlot_ = (ModuloTimeSlot) value;
  }
  public ModuloTimeSlot setReferenceTimeSlotToNewInstance() {
    referenceTimeSlot_ = new ModuloTimeSlot();
    return referenceTimeSlot_;
  }
  
  private TOA_MeasurementsOfRef toaMeasurementsOfRef_;
  public TOA_MeasurementsOfRef getToaMeasurementsOfRef() {
    return toaMeasurementsOfRef_;
  }
  /**
   * @throws ClassCastException if value is not a TOA_MeasurementsOfRef
   */
  public void setToaMeasurementsOfRef(Asn1Object value) {
    this.toaMeasurementsOfRef_ = (TOA_MeasurementsOfRef) value;
  }
  public TOA_MeasurementsOfRef setToaMeasurementsOfRefToNewInstance() {
    toaMeasurementsOfRef_ = new TOA_MeasurementsOfRef();
    return toaMeasurementsOfRef_;
  }
  
  private StdResolution stdResolution_;
  public StdResolution getStdResolution() {
    return stdResolution_;
  }
  /**
   * @throws ClassCastException if value is not a StdResolution
   */
  public void setStdResolution(Asn1Object value) {
    this.stdResolution_ = (StdResolution) value;
  }
  public StdResolution setStdResolutionToNewInstance() {
    stdResolution_ = new StdResolution();
    return stdResolution_;
  }
  
  private OTD_MsrElementFirst.taCorrectionType taCorrection_;
  public OTD_MsrElementFirst.taCorrectionType getTaCorrection() {
    return taCorrection_;
  }
  /**
   * @throws ClassCastException if value is not a OTD_MsrElementFirst.taCorrectionType
   */
  public void setTaCorrection(Asn1Object value) {
    this.taCorrection_ = (OTD_MsrElementFirst.taCorrectionType) value;
  }
  public OTD_MsrElementFirst.taCorrectionType setTaCorrectionToNewInstance() {
    taCorrection_ = new OTD_MsrElementFirst.taCorrectionType();
    return taCorrection_;
  }
  
  private SeqOfOTD_FirstSetMsrs otd_FirstSetMsrs_;
  public SeqOfOTD_FirstSetMsrs getOtd_FirstSetMsrs() {
    return otd_FirstSetMsrs_;
  }
  /**
   * @throws ClassCastException if value is not a SeqOfOTD_FirstSetMsrs
   */
  public void setOtd_FirstSetMsrs(Asn1Object value) {
    this.otd_FirstSetMsrs_ = (SeqOfOTD_FirstSetMsrs) value;
  }
  public SeqOfOTD_FirstSetMsrs setOtd_FirstSetMsrsToNewInstance() {
    otd_FirstSetMsrs_ = new SeqOfOTD_FirstSetMsrs();
    return otd_FirstSetMsrs_;
  }
  

  

  

  @Override public Iterable<? extends SequenceComponent> getComponents() {
    ImmutableList.Builder<SequenceComponent> builder = ImmutableList.builder();
    
    builder.add(new SequenceComponent() {
          Asn1Tag tag = Asn1Tag.fromClassAndNumber(2, 0);

          @Override public boolean isExplicitlySet() {
            return getRefFrameNumber() != null;
          }

          @Override public boolean hasDefaultValue() {
            return false;
          }

          @Override public boolean isOptional() {
            return false;
          }

          @Override public Asn1Object getComponentValue() {
            return getRefFrameNumber();
          }

          @Override public void setToNewInstance() {
            setRefFrameNumberToNewInstance();
          }

          @Override public Collection<Asn1Tag> getPossibleFirstTags() {
            return tag == null ? OTD_MsrElementFirst.refFrameNumberType.getPossibleFirstTags() : ImmutableList.of(tag);
          }

          @Override
          public Asn1Tag getTag() {
            return tag;
          }

          @Override
          public boolean isImplicitTagging() {
            return true;
          }

          @Override public String toIndentedString(String indent) {
                return "refFrameNumber : "
                    + getRefFrameNumber().toIndentedString(indent);
              }
        });
    
    builder.add(new SequenceComponent() {
          Asn1Tag tag = Asn1Tag.fromClassAndNumber(2, 1);

          @Override public boolean isExplicitlySet() {
            return getReferenceTimeSlot() != null;
          }

          @Override public boolean hasDefaultValue() {
            return false;
          }

          @Override public boolean isOptional() {
            return false;
          }

          @Override public Asn1Object getComponentValue() {
            return getReferenceTimeSlot();
          }

          @Override public void setToNewInstance() {
            setReferenceTimeSlotToNewInstance();
          }

          @Override public Collection<Asn1Tag> getPossibleFirstTags() {
            return tag == null ? ModuloTimeSlot.getPossibleFirstTags() : ImmutableList.of(tag);
          }

          @Override
          public Asn1Tag getTag() {
            return tag;
          }

          @Override
          public boolean isImplicitTagging() {
            return true;
          }

          @Override public String toIndentedString(String indent) {
                return "referenceTimeSlot : "
                    + getReferenceTimeSlot().toIndentedString(indent);
              }
        });
    
    builder.add(new SequenceComponent() {
          Asn1Tag tag = Asn1Tag.fromClassAndNumber(2, 2);

          @Override public boolean isExplicitlySet() {
            return getToaMeasurementsOfRef() != null;
          }

          @Override public boolean hasDefaultValue() {
            return false;
          }

          @Override public boolean isOptional() {
            return true;
          }

          @Override public Asn1Object getComponentValue() {
            return getToaMeasurementsOfRef();
          }

          @Override public void setToNewInstance() {
            setToaMeasurementsOfRefToNewInstance();
          }

          @Override public Collection<Asn1Tag> getPossibleFirstTags() {
            return tag == null ? TOA_MeasurementsOfRef.getPossibleFirstTags() : ImmutableList.of(tag);
          }

          @Override
          public Asn1Tag getTag() {
            return tag;
          }

          @Override
          public boolean isImplicitTagging() {
            return true;
          }

          @Override public String toIndentedString(String indent) {
                return "toaMeasurementsOfRef : "
                    + getToaMeasurementsOfRef().toIndentedString(indent);
              }
        });
    
    builder.add(new SequenceComponent() {
          Asn1Tag tag = Asn1Tag.fromClassAndNumber(2, 3);

          @Override public boolean isExplicitlySet() {
            return getStdResolution() != null;
          }

          @Override public boolean hasDefaultValue() {
            return false;
          }

          @Override public boolean isOptional() {
            return false;
          }

          @Override public Asn1Object getComponentValue() {
            return getStdResolution();
          }

          @Override public void setToNewInstance() {
            setStdResolutionToNewInstance();
          }

          @Override public Collection<Asn1Tag> getPossibleFirstTags() {
            return tag == null ? StdResolution.getPossibleFirstTags() : ImmutableList.of(tag);
          }

          @Override
          public Asn1Tag getTag() {
            return tag;
          }

          @Override
          public boolean isImplicitTagging() {
            return true;
          }

          @Override public String toIndentedString(String indent) {
                return "stdResolution : "
                    + getStdResolution().toIndentedString(indent);
              }
        });
    
    builder.add(new SequenceComponent() {
          Asn1Tag tag = Asn1Tag.fromClassAndNumber(2, 4);

          @Override public boolean isExplicitlySet() {
            return getTaCorrection() != null;
          }

          @Override public boolean hasDefaultValue() {
            return false;
          }

          @Override public boolean isOptional() {
            return true;
          }

          @Override public Asn1Object getComponentValue() {
            return getTaCorrection();
          }

          @Override public void setToNewInstance() {
            setTaCorrectionToNewInstance();
          }

          @Override public Collection<Asn1Tag> getPossibleFirstTags() {
            return tag == null ? OTD_MsrElementFirst.taCorrectionType.getPossibleFirstTags() : ImmutableList.of(tag);
          }

          @Override
          public Asn1Tag getTag() {
            return tag;
          }

          @Override
          public boolean isImplicitTagging() {
            return true;
          }

          @Override public String toIndentedString(String indent) {
                return "taCorrection : "
                    + getTaCorrection().toIndentedString(indent);
              }
        });
    
    builder.add(new SequenceComponent() {
          Asn1Tag tag = Asn1Tag.fromClassAndNumber(2, 5);

          @Override public boolean isExplicitlySet() {
            return getOtd_FirstSetMsrs() != null;
          }

          @Override public boolean hasDefaultValue() {
            return false;
          }

          @Override public boolean isOptional() {
            return true;
          }

          @Override public Asn1Object getComponentValue() {
            return getOtd_FirstSetMsrs();
          }

          @Override public void setToNewInstance() {
            setOtd_FirstSetMsrsToNewInstance();
          }

          @Override public Collection<Asn1Tag> getPossibleFirstTags() {
            return tag == null ? SeqOfOTD_FirstSetMsrs.getPossibleFirstTags() : ImmutableList.of(tag);
          }

          @Override
          public Asn1Tag getTag() {
            return tag;
          }

          @Override
          public boolean isImplicitTagging() {
            return true;
          }

          @Override public String toIndentedString(String indent) {
                return "otd_FirstSetMsrs : "
                    + getOtd_FirstSetMsrs().toIndentedString(indent);
              }
        });
    
    return builder.build();
  }

  @Override public Iterable<? extends SequenceComponent>
                                                    getExtensionComponents() {
    ImmutableList.Builder<SequenceComponent> builder = ImmutableList.builder();
      
      return builder.build();
    }

  
/*
 */


//

/**
 */
public static class refFrameNumberType extends Asn1Integer {
  //

  private static final Asn1Tag TAG_refFrameNumberType
      = Asn1Tag.fromClassAndNumber(-1, -1);

  public refFrameNumberType() {
    super();
    setValueRange("0", "42431");

  }

  @Override
  @Nullable
  protected Asn1Tag getTag() {
    return TAG_refFrameNumberType;
  }

  @Override
  protected boolean isTagImplicit() {
    return true;
  }

  public static Collection<Asn1Tag> getPossibleFirstTags() {
    if (TAG_refFrameNumberType != null) {
      return ImmutableList.of(TAG_refFrameNumberType);
    } else {
      return Asn1Integer.getPossibleFirstTags();
    }
  }

  /**
   * Creates a new refFrameNumberType from encoded stream.
   */
  public static refFrameNumberType fromPerUnaligned(byte[] encodedBytes) {
    refFrameNumberType result = new refFrameNumberType();
    result.decodePerUnaligned(new BitStreamReader(encodedBytes));
    return result;
  }

  /**
   * Creates a new refFrameNumberType from encoded stream.
   */
  public static refFrameNumberType fromPerAligned(byte[] encodedBytes) {
    refFrameNumberType result = new refFrameNumberType();
    result.decodePerAligned(new BitStreamReader(encodedBytes));
    return result;
  }

  @Override public Iterable<BitStream> encodePerUnaligned() {
    return super.encodePerUnaligned();
  }

  @Override public Iterable<BitStream> encodePerAligned() {
    return super.encodePerAligned();
  }

  @Override public void decodePerUnaligned(BitStreamReader reader) {
    super.decodePerUnaligned(reader);
  }

  @Override public void decodePerAligned(BitStreamReader reader) {
    super.decodePerAligned(reader);
  }

  @Override public String toString() {
    return toIndentedString("");
  }

  public String toIndentedString(String indent) {
    return "refFrameNumberType = " + getInteger() + ";\n";
  }
}

  
  
  
  
  
  
  
/*
 */


//

/**
 */
public static class taCorrectionType extends Asn1Integer {
  //

  private static final Asn1Tag TAG_taCorrectionType
      = Asn1Tag.fromClassAndNumber(-1, -1);

  public taCorrectionType() {
    super();
    setValueRange("0", "960");

  }

  @Override
  @Nullable
  protected Asn1Tag getTag() {
    return TAG_taCorrectionType;
  }

  @Override
  protected boolean isTagImplicit() {
    return true;
  }

  public static Collection<Asn1Tag> getPossibleFirstTags() {
    if (TAG_taCorrectionType != null) {
      return ImmutableList.of(TAG_taCorrectionType);
    } else {
      return Asn1Integer.getPossibleFirstTags();
    }
  }

  /**
   * Creates a new taCorrectionType from encoded stream.
   */
  public static taCorrectionType fromPerUnaligned(byte[] encodedBytes) {
    taCorrectionType result = new taCorrectionType();
    result.decodePerUnaligned(new BitStreamReader(encodedBytes));
    return result;
  }

  /**
   * Creates a new taCorrectionType from encoded stream.
   */
  public static taCorrectionType fromPerAligned(byte[] encodedBytes) {
    taCorrectionType result = new taCorrectionType();
    result.decodePerAligned(new BitStreamReader(encodedBytes));
    return result;
  }

  @Override public Iterable<BitStream> encodePerUnaligned() {
    return super.encodePerUnaligned();
  }

  @Override public Iterable<BitStream> encodePerAligned() {
    return super.encodePerAligned();
  }

  @Override public void decodePerUnaligned(BitStreamReader reader) {
    super.decodePerUnaligned(reader);
  }

  @Override public void decodePerAligned(BitStreamReader reader) {
    super.decodePerAligned(reader);
  }

  @Override public String toString() {
    return toIndentedString("");
  }

  public String toIndentedString(String indent) {
    return "taCorrectionType = " + getInteger() + ";\n";
  }
}

  
  
  

    

  @Override public Iterable<BitStream> encodePerUnaligned() {
    return super.encodePerUnaligned();
  }

  @Override public Iterable<BitStream> encodePerAligned() {
    return super.encodePerAligned();
  }

  @Override public void decodePerUnaligned(BitStreamReader reader) {
    super.decodePerUnaligned(reader);
  }

  @Override public void decodePerAligned(BitStreamReader reader) {
    super.decodePerAligned(reader);
  }

  @Override public String toString() {
    return toIndentedString("");
  }

  public String toIndentedString(String indent) {
    StringBuilder builder = new StringBuilder();
    builder.append("OTD_MsrElementFirst = {\n");
    final String internalIndent = indent + "  ";
    for (SequenceComponent component : getComponents()) {
      if (component.isExplicitlySet()) {
        builder.append(internalIndent)
            .append(component.toIndentedString(internalIndent));
      }
    }
    if (isExtensible()) {
      builder.append(internalIndent).append("...\n");
      for (SequenceComponent component : getExtensionComponents()) {
        if (component.isExplicitlySet()) {
          builder.append(internalIndent)
              .append(component.toIndentedString(internalIndent));
        }
      }
    }
    builder.append(indent).append("};\n");
    return builder.toString();
  }
}
