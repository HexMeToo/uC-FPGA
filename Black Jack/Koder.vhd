library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Koder is
	port(	CLK: in std_logic;
			D: in std_logic_vector(3 downto 0);
			Q: buffer std_logic_vector(3 downto 0) );
end Koder;

architecture Behavioral of Koder is
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			case D is
				when "0010" => Q <="0000";
				when "0011" => Q <="0001";
				when "0100" => Q <="0010";
				when "0101" => Q <="0011";
				when "0110" => Q <="0100";
				when "0111" => Q <="0101";
				when "1000" => Q <="0110";
				when "1001" => Q <="0111";
				when "1010" => Q <="1000";
				when others => Q <="0000";
			end case;
		end if;
	end process;
end Behavioral;