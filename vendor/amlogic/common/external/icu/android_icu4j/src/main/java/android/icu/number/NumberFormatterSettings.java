/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package android.icu.number;

import android.icu.impl.number.MacroProps;
import android.icu.impl.number.Padder;
import android.icu.number.NumberFormatter.DecimalSeparatorDisplay;
import android.icu.number.NumberFormatter.SignDisplay;
import android.icu.number.NumberFormatter.UnitWidth;
import android.icu.text.DecimalFormatSymbols;
import android.icu.text.NumberingSystem;
import android.icu.util.Currency;
import android.icu.util.Measure;
import android.icu.util.MeasureUnit;
import android.icu.util.NoUnit;
import android.icu.util.ULocale;

/**
 * An abstract base class for specifying settings related to number formatting. This class is implemented by
 * {@link UnlocalizedNumberFormatter} and {@link LocalizedNumberFormatter}. This class is not intended for public
 * subclassing.
 *
 * @see NumberFormatter
 * @hide Only a subset of ICU is exposed in Android
 * @hide draft / provisional / internal are hidden on Android
 */
public abstract class NumberFormatterSettings<T extends NumberFormatterSettings<?>> {

    static final int KEY_MACROS = 0;
    static final int KEY_LOCALE = 1;
    static final int KEY_NOTATION = 2;
    static final int KEY_UNIT = 3;
    static final int KEY_ROUNDER = 4;
    static final int KEY_GROUPER = 5;
    static final int KEY_PADDER = 6;
    static final int KEY_INTEGER = 7;
    static final int KEY_SYMBOLS = 8;
    static final int KEY_UNIT_WIDTH = 9;
    static final int KEY_SIGN = 10;
    static final int KEY_DECIMAL = 11;
    static final int KEY_THRESHOLD = 12;
    static final int KEY_MAX = 13;

    final NumberFormatterSettings<?> parent;
    final int key;
    final Object value;
    volatile MacroProps resolvedMacros;

    NumberFormatterSettings(NumberFormatterSettings<?> parent, int key, Object value) {
        this.parent = parent;
        this.key = key;
        this.value = value;
    }

    /**
     * Specifies the notation style (simple, scientific, or compact) for rendering numbers.
     *
     * <ul>
     * <li>Simple notation: "12,300"
     * <li>Scientific notation: "1.23E4"
     * <li>Compact notation: "12K"
     * </ul>
     *
     * <p>
     * All notation styles will be properly localized with locale data, and all notation styles are compatible with
     * units, rounding strategies, and other number formatter settings.
     *
     * <p>
     * Pass this method the return value of a {@link Notation} factory method. For example:
     *
     * <pre>
     * NumberFormatter.with().notation(Notation.compactShort())
     * </pre>
     *
     * The default is to use simple notation.
     *
     * @param notation
     *            The notation strategy to use.
     * @return The fluent chain.
     * @see Notation
     * @hide draft / provisional / internal are hidden on Android
     */
    public T notation(Notation notation) {
        return create(KEY_NOTATION, notation);
    }

    /**
     * Specifies the unit (unit of measure, currency, or percent) to associate with rendered numbers.
     *
     * <ul>
     * <li>Unit of measure: "12.3 meters"
     * <li>Currency: "$12.30"
     * <li>Percent: "12.3%"
     * </ul>
     *
     * <p>
     * <strong>Note:</strong> The unit can also be specified by passing a {@link Measure} to
     * {@link LocalizedNumberFormatter#format(Measure)}. Units specified via the format method take precedence over
     * units specified here. This setter is designed for situations when the unit is constant for the duration of the
     * number formatting process.
     *
     * <p>
     * All units will be properly localized with locale data, and all units are compatible with notation styles,
     * rounding strategies, and other number formatter settings.
     *
     * <p>
     * Pass this method any instance of {@link MeasureUnit}. For units of measure:
     *
     * <pre>
     * NumberFormatter.with().unit(MeasureUnit.METER)
     * </pre>
     *
     * Currency:
     *
     * <pre>
     * NumberFormatter.with().unit(Currency.getInstance("USD"))
     * </pre>
     *
     * Percent:
     *
     * <pre>
     * NumberFormatter.with().unit(NoUnit.PERCENT)
     * </pre>
     *
     * The default is to render without units (equivalent to {@link NoUnit#BASE}).
     *
     * @param unit
     *            The unit to render.
     * @return The fluent chain.
     * @see MeasureUnit
     * @see Currency
     * @see NoUnit
     * @hide draft / provisional / internal are hidden on Android
     */
    public T unit(MeasureUnit unit) {
        return create(KEY_UNIT, unit);
    }

    /**
     * Specifies the rounding strategy to use when formatting numbers.
     *
     * <ul>
     * <li>Round to 3 decimal places: "3.142"
     * <li>Round to 3 significant figures: "3.14"
     * <li>Round to the closest nickel: "3.15"
     * <li>Do not perform rounding: "3.1415926..."
     * </ul>
     *
     * <p>
     * Pass this method the return value of one of the factory methods on {@link Rounder}. For example:
     *
     * <pre>
     * NumberFormatter.with().rounding(Rounder.fixedFraction(2))
     * </pre>
     *
     * <p>
     * In most cases, the default rounding strategy is to round to 6 fraction places; i.e.,
     * <code>Rounder.maxFraction(6)</code>. The exceptions are if compact notation is being used, then the compact
     * notation rounding strategy is used (see {@link Notation#compactShort} for details), or if the unit is a currency,
     * then standard currency rounding is used, which varies from currency to currency (see {@link Rounder#currency} for
     * details).
     *
     * @param rounder
     *            The rounding strategy to use.
     * @return The fluent chain.
     * @see Rounder
     * @hide draft / provisional / internal are hidden on Android
     */
    public T rounding(Rounder rounder) {
        return create(KEY_ROUNDER, rounder);
    }

    /**
     * Specifies the grouping strategy to use when formatting numbers.
     *
     * <ul>
     * <li>Default grouping: "12,300" and "1,230"
     * <li>Grouping with at least 2 digits: "12,300" and "1230"
     * <li>No grouping: "12300" and "1230"
     * </ul>
     *
     * <p>
     * The exact grouping widths will be chosen based on the locale.
     *
     * <p>
     * Pass this method the return value of one of the factory methods on {@link Grouper}. For example:
     *
     * <pre>
     * NumberFormatter.with().grouping(Grouper.min2())
     * </pre>
     *
     * The default is to perform grouping without concern for the minimum grouping digits.
     *
     * @param grouper
     *            The grouping strategy to use.
     * @return The fluent chain.
     * @see Grouper
     * @see Notation
     * @deprecated ICU 60 This API is technical preview; see #7861.
     * @hide draft / provisional / internal are hidden on Android
     */
    @Deprecated
    public T grouping(Grouper grouper) {
        return create(KEY_GROUPER, grouper);
    }

    /**
     * Specifies the minimum and maximum number of digits to render before the decimal mark.
     *
     * <ul>
     * <li>Zero minimum integer digits: ".08"
     * <li>One minimum integer digit: "0.08"
     * <li>Two minimum integer digits: "00.08"
     * </ul>
     *
     * <p>
     * Pass this method the return value of {@link IntegerWidth#zeroFillTo(int)}. For example:
     *
     * <pre>
     * NumberFormatter.with().integerWidth(IntegerWidth.zeroFillTo(2))
     * </pre>
     *
     * The default is to have one minimum integer digit.
     *
     * @param style
     *            The integer width to use.
     * @return The fluent chain.
     * @see IntegerWidth
     * @hide draft / provisional / internal are hidden on Android
     */
    public T integerWidth(IntegerWidth style) {
        return create(KEY_INTEGER, style);
    }

    /**
     * Specifies the symbols (decimal separator, grouping separator, percent sign, numerals, etc.) to use when rendering
     * numbers.
     *
     * <ul>
     * <li><em>en_US</em> symbols: "12,345.67"
     * <li><em>fr_FR</em> symbols: "12&nbsp;345,67"
     * <li><em>de_CH</em> symbols: "12’345.67"
     * <li><em>my_MY</em> symbols: "၁၂,၃၄၅.၆၇"
     * </ul>
     *
     * <p>
     * Pass this method an instance of {@link DecimalFormatSymbols}. For example:
     *
     * <pre>
     * NumberFormatter.with().symbols(DecimalFormatSymbols.getInstance(new ULocale("de_CH")))
     * </pre>
     *
     * <p>
     * <strong>Note:</strong> DecimalFormatSymbols automatically chooses the best numbering system based on the locale.
     * In the examples above, the first three are using the Latin numbering system, and the fourth is using the Myanmar
     * numbering system.
     *
     * <p>
     * <strong>Note:</strong> The instance of DecimalFormatSymbols will be copied: changes made to the symbols object
     * after passing it into the fluent chain will not be seen.
     *
     * <p>
     * <strong>Note:</strong> Calling this method will override the NumberingSystem previously specified in
     * {@link #symbols(NumberingSystem)}.
     *
     * <p>
     * The default is to choose the symbols based on the locale specified in the fluent chain.
     *
     * @param symbols
     *            The DecimalFormatSymbols to use.
     * @return The fluent chain.
     * @see DecimalFormatSymbols
     * @hide draft / provisional / internal are hidden on Android
     */
    public T symbols(DecimalFormatSymbols symbols) {
        symbols = (DecimalFormatSymbols) symbols.clone();
        return create(KEY_SYMBOLS, symbols);
    }

    /**
     * Specifies that the given numbering system should be used when fetching symbols.
     *
     * <ul>
     * <li>Latin numbering system: "12,345"
     * <li>Myanmar numbering system: "၁၂,၃၄၅"
     * <li>Math Sans Bold numbering system: "𝟭𝟮,𝟯𝟰𝟱"
     * </ul>
     *
     * <p>
     * Pass this method an instance of {@link NumberingSystem}. For example, to force the locale to always use the Latin
     * alphabet numbering system (ASCII digits):
     *
     * <pre>
     * NumberFormatter.with().symbols(NumberingSystem.LATIN)
     * </pre>
     *
     * <p>
     * <strong>Note:</strong> Calling this method will override the DecimalFormatSymbols previously specified in
     * {@link #symbols(DecimalFormatSymbols)}.
     *
     * <p>
     * The default is to choose the best numbering system for the locale.
     *
     * @param ns
     *            The NumberingSystem to use.
     * @return The fluent chain.
     * @see NumberingSystem
     * @hide draft / provisional / internal are hidden on Android
     */
    public T symbols(NumberingSystem ns) {
        return create(KEY_SYMBOLS, ns);
    }

    /**
     * Sets the width of the unit (measure unit or currency). Most common values:
     *
     * <ul>
     * <li>Short: "$12.00", "12 m"
     * <li>ISO Code: "USD 12.00"
     * <li>Full name: "12.00 US dollars", "12 meters"
     * </ul>
     *
     * <p>
     * Pass an element from the {@link UnitWidth} enum to this setter. For example:
     *
     * <pre>
     * NumberFormatter.with().unitWidth(UnitWidth.FULL_NAME)
     * </pre>
     *
     * <p>
     * The default is the SHORT width.
     *
     * @param style
     *            The width to use when rendering numbers.
     * @return The fluent chain
     * @see UnitWidth
     * @hide draft / provisional / internal are hidden on Android
     */
    public T unitWidth(UnitWidth style) {
        return create(KEY_UNIT_WIDTH, style);
    }

    /**
     * Sets the plus/minus sign display strategy. Most common values:
     *
     * <ul>
     * <li>Auto: "123", "-123"
     * <li>Always: "+123", "-123"
     * <li>Accounting: "$123", "($123)"
     * </ul>
     *
     * <p>
     * Pass an element from the {@link SignDisplay} enum to this setter. For example:
     *
     * <pre>
     * NumberFormatter.with().sign(SignDisplay.ALWAYS)
     * </pre>
     *
     * <p>
     * The default is AUTO sign display.
     *
     * @param style
     *            The sign display strategy to use when rendering numbers.
     * @return The fluent chain
     * @see SignDisplay
     * @hide draft / provisional / internal are hidden on Android
     */
    public T sign(SignDisplay style) {
        return create(KEY_SIGN, style);
    }

    /**
     * Sets the decimal separator display strategy. This affects integer numbers with no fraction part. Most common
     * values:
     *
     * <ul>
     * <li>Auto: "1"
     * <li>Always: "1."
     * </ul>
     *
     * <p>
     * Pass an element from the {@link DecimalSeparatorDisplay} enum to this setter. For example:
     *
     * <pre>
     * NumberFormatter.with().decimal(DecimalSeparatorDisplay.ALWAYS)
     * </pre>
     *
     * <p>
     * The default is AUTO decimal separator display.
     *
     * @param style
     *            The decimal separator display strategy to use when rendering numbers.
     * @return The fluent chain
     * @see DecimalSeparatorDisplay
     * @hide draft / provisional / internal are hidden on Android
     */
    public T decimal(DecimalSeparatorDisplay style) {
        return create(KEY_DECIMAL, style);
    }

    /**
     * Internal method to set a starting macros.
     *
     * @deprecated ICU 60 This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on Android
     */
    @Deprecated
    public T macros(MacroProps macros) {
        return create(KEY_MACROS, macros);
    }

    /**
     * Set the padding strategy. May be added to ICU 61; see #13338.
     *
     * @deprecated ICU 60 This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on Android
     */
    @Deprecated
    public T padding(Padder padder) {
        return create(KEY_PADDER, padder);
    }

    /**
     * Internal fluent setter to support a custom regulation threshold. A threshold of 1 causes the data structures to
     * be built right away. A threshold of 0 prevents the data structures from being built.
     *
     * @deprecated ICU 60 This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on Android
     */
    @Deprecated
    public T threshold(Long threshold) {
        return create(KEY_THRESHOLD, threshold);
    }

    /* package-protected */ abstract T create(int key, Object value);

    MacroProps resolve() {
        if (resolvedMacros != null) {
            return resolvedMacros;
        }
        // Although the linked-list fluent storage approach requires this method,
        // my benchmarks show that linked-list is still faster than a full clone
        // of a MacroProps object at each step.
        // TODO: Remove the reference to the parent after the macros are resolved?
        MacroProps macros = new MacroProps();
        NumberFormatterSettings<?> current = this;
        while (current != null) {
            switch (current.key) {
            case KEY_MACROS:
                macros.fallback((MacroProps) current.value);
                break;
            case KEY_LOCALE:
                if (macros.loc == null) {
                    macros.loc = (ULocale) current.value;
                }
                break;
            case KEY_NOTATION:
                if (macros.notation == null) {
                    macros.notation = (Notation) current.value;
                }
                break;
            case KEY_UNIT:
                if (macros.unit == null) {
                    macros.unit = (MeasureUnit) current.value;
                }
                break;
            case KEY_ROUNDER:
                if (macros.rounder == null) {
                    macros.rounder = (Rounder) current.value;
                }
                break;
            case KEY_GROUPER:
                if (macros.grouper == null) {
                    macros.grouper = (Grouper) current.value;
                }
                break;
            case KEY_PADDER:
                if (macros.padder == null) {
                    macros.padder = (Padder) current.value;
                }
                break;
            case KEY_INTEGER:
                if (macros.integerWidth == null) {
                    macros.integerWidth = (IntegerWidth) current.value;
                }
                break;
            case KEY_SYMBOLS:
                if (macros.symbols == null) {
                    macros.symbols = /* (Object) */ current.value;
                }
                break;
            case KEY_UNIT_WIDTH:
                if (macros.unitWidth == null) {
                    macros.unitWidth = (UnitWidth) current.value;
                }
                break;
            case KEY_SIGN:
                if (macros.sign == null) {
                    macros.sign = (SignDisplay) current.value;
                }
                break;
            case KEY_DECIMAL:
                if (macros.decimal == null) {
                    macros.decimal = (DecimalSeparatorDisplay) current.value;
                }
                break;
            case KEY_THRESHOLD:
                if (macros.threshold == null) {
                    macros.threshold = (Long) current.value;
                }
                break;
            default:
                throw new AssertionError("Unknown key: " + current.key);
            }
            current = current.parent;
        }
        resolvedMacros = macros;
        return macros;
    }

    /**
     * {@inheritDoc}
     *
     * @hide draft / provisional / internal are hidden on Android
     */
    @Override
    public int hashCode() {
        return resolve().hashCode();
    }

    /**
     * {@inheritDoc}
     *
     * @hide draft / provisional / internal are hidden on Android
     */
    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }
        if (other == null) {
            return false;
        }
        if (!(other instanceof NumberFormatterSettings)) {
            return false;
        }
        return resolve().equals(((NumberFormatterSettings<?>) other).resolve());
    }
}
